#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <algorithm>
#include <execution>

#include "Renderer.h"

namespace RT::Render
{

    Renderer::Renderer()
        : m_MainView(), m_FrameIndex(0), Accumulate(true), m_ViewCash()
        , m_ActiveCamera(nullptr), m_ActiveScene(nullptr)
        , m_CashedCoord(0)
    {
        m_MainView.SetFormat(Image::Format::RGBA32F);
    }

    void Renderer::OnResize(int32_t width, int32_t height)
    {
        if (m_MainView.GetWidth() != width || m_MainView.GetHeight() != height)
        {
            ResetFrame();
            m_MainView.Resize(width, height);
            m_ViewCash.resize(m_MainView.GetWidth() * m_MainView.GetHeight());
            m_AccumulatedView.resize(m_MainView.GetWidth() * m_MainView.GetHeight());

            m_CashedCoord.resize(height);
            for (int32_t y = 0; y < height; y++) m_CashedCoord[y] = y;
        }
    }

    void Renderer::Render(const Camera& camera, const Scene& scene)
    {
        m_ActiveCamera = &camera;
        m_ActiveScene = &scene;

        m_FrameIndex++;
        if (!Accumulate)
            m_FrameIndex = 1;

        if (m_FrameIndex == 1)
            std::memset(m_AccumulatedView.data(), 0, m_MainView.GetWidth() * m_MainView.GetHeight() * sizeof(glm::vec4));

        std::for_each(std::execution::par, m_CashedCoord.begin(), m_CashedCoord.end(), [this](int32_t y)
        {
            for (int32_t x = 0; x < m_MainView.GetWidth(); x++)
            {
                m_AccumulatedView[y * m_MainView.GetWidth() + x] += PixelColor({ x, y });
             
                glm::vec4 accumulatedColor = m_AccumulatedView[y * m_MainView.GetWidth() + x] / (float)m_FrameIndex;

                m_ViewCash[y * m_MainView.GetWidth() + x] = glm::clamp(accumulatedColor, glm::vec4(0), glm::vec4(1));
            }
        });


        m_MainView.Update(m_ViewCash.data());
    }

    glm::vec4 Renderer::PixelColor(glm::ivec2 pixel) const
    {
        constexpr int32_t maxBounces = 6;
        uint32_t localSeed = ((pixel.y * m_MainView.GetWidth() + pixel.x) << 4 ^ 1252314u) * m_FrameIndex;

        Ray ray = { 
            m_ActiveCamera->GetPosition(),
            m_ActiveCamera->GetCoords()[pixel.y * m_MainView.GetWidth() + pixel.x]
        };

        glm::vec3 finalColor = glm::vec3(0);
        glm::vec3 contribution = glm::vec3(1);

        for (int32_t i = 1; i < maxBounces; i++)
        {
            Payload payload = TraceRay(ray);
            localSeed *= i;

            if (payload.ObjectId == -1)
            {
                finalColor = finalColor + glm::vec3(payload.Color) * contribution;
                break;
            }

            const Sphere& closestSphere = m_ActiveScene->Spheres[payload.ObjectId];
            const Material& sphereMaterial = m_ActiveScene->Materials[closestSphere.MaterialId];

            contribution *= sphereMaterial.Albedo;
            finalColor += GetEmmision(sphereMaterial);

            ray.Origin = payload.HitPosition + payload.HitNormal * 0.0001f;

            //glm::vec3 unitSphere = glm::vec3(FastRandom(localSeed), FastRandom(localSeed), FastRandom(localSeed)) - 0.5f;
            //ray.Direction = glm::reflect(ray.Direction, payload.HitNormal + unitSphere * sphereMaterial.Roughness);
            glm::vec3 unitSphere = 2.0f * glm::vec3(FastRandom(localSeed), FastRandom(localSeed), FastRandom(localSeed)) - 1.0f;
            ray.Direction = glm::normalize(payload.HitNormal + unitSphere);
        }
        return glm::vec4(finalColor, 1.0f);
    }

    Renderer::Payload Renderer::TraceRay(const Ray& ray) const
    {
        float closetsDistant = std::numeric_limits<float>::max();
        int32_t closestHitId = -1;
        for (int32_t sphereId = 0; sphereId < m_ActiveScene->Spheres.size(); sphereId++)
        {
            const Sphere& sphere = m_ActiveScene->Spheres[sphereId];

            glm::vec3 origin = ray.Origin - sphere.Position;

            float a = glm::dot(ray.Direction, ray.Direction);
            float b = 2 * glm::dot(origin, ray.Direction);
            float c = glm::dot(origin, origin) - sphere.Radius * sphere.Radius;

            float delta = b * b - 4 * a * c;
            if (delta < 0)
                continue;

            float t = (-b - glm::sqrt(delta)) / (2.0f * a);
            if (t >= 0.0 && t < closetsDistant)
            {
                closetsDistant= t;
                closestHitId = sphereId;
            }
        }

        if (closestHitId == -1)
            return Miss(ray);

        return ClosestHit(ray, closestHitId, closetsDistant);
    }

    Renderer::Payload Renderer::ClosestHit(const Ray& ray, int32_t objectId, float hitDistance) const
    {
        const Sphere& sphere = m_ActiveScene->Spheres[objectId];
        const Material& material = m_ActiveScene->Materials[sphere.MaterialId];

        Payload payload;
        payload.HitPosition = ray.Origin + hitDistance * ray.Direction;
        payload.HitNormal = glm::normalize(payload.HitPosition - sphere.Position);
        payload.HitDistance = hitDistance;
        payload.ObjectId = objectId;

        //glm::vec3 lightDir = glm::normalize(glm::vec3(-1, -1, -1));
        //float intensity = glm::clamp(glm::dot(payload.HitNormal, -lightDir), 0.0f, 1.0f);
        //payload.Color = glm::vec4(material.Albedo * intensity, 1.0f);
        payload.Color = glm::vec4(material.Albedo, 1.0f);

        return payload;
    }

    Renderer::Payload Renderer::Miss(const Ray& ray) const
    {
        Payload payload = {
            glm::vec4{ 0.6f, 0.7f, 0.9f, 1 },
            glm::vec3{ 0 },
            glm::vec3{ 0 },
            std::numeric_limits<float>::max(),
            -1
        };
        return payload;
    }

    static inline uint32_t pcg_hash(uint32_t input)
    {
        uint32_t state = input * 747796405u + 2891336453u;
        uint32_t word = ((state >> ((state >> 28u) + 4u)) ^ state) * 277803737u;
        return (word >> 22u) ^ word;
    }

    float FastRandom(uint32_t& seed)
    {
        seed = pcg_hash(seed);
        return (float)seed / (float)std::numeric_limits<uint32_t>::max();
    }

}
