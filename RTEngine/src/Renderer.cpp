#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <algorithm>
#include <execution>

#include "Renderer.h"

namespace RT::Render
{
    
    Renderer::Renderer()
        : m_FrameIndex(0), m_Seed(0), m_MainView(), m_ViewCash()
        , m_ActiveCamera(nullptr), m_ActiveScene(nullptr)
        , m_CashedCoord(0)
    {
        m_MainView.SetFormat(Image::Format::RGBA32F);
    }

    void Renderer::OnResize(int32_t width, int32_t height)
    {
        if (m_MainView.GetWidth() != width || m_MainView.GetHeight() != height)
        {
            m_FrameIndex = 0;
            m_MainView.Resize(width, height);
            m_ViewCash.resize(m_MainView.GetWidth() * m_MainView.GetHeight());

            m_CashedCoord.resize(height);
            for (int32_t y = 0; y < height; y++) m_CashedCoord[y] = y;
        }
    }

    void Renderer::Render(const Camera& camera, const Scene& scene)
    {
        m_ActiveCamera = &camera;
        m_ActiveScene = &scene;

        m_FrameIndex += 1;
        m_Seed += m_FrameIndex;

        std::for_each(std::execution::par, m_CashedCoord.begin(), m_CashedCoord.end(), [this](int32_t y)
        {
            for (int32_t x = 0; x < m_MainView.GetWidth(); x++)
            {
                m_ViewCash[y * m_MainView.GetWidth() + x] = PixelColor({ x, y });
            }
        });
        m_MainView.Update(m_ViewCash.data());
    }

    glm::vec4 Renderer::PixelColor(glm::vec2 pixel)
    {
        glm::vec3 rayDirection = m_ActiveCamera->GetCoords()[pixel.y * m_MainView.GetWidth() + pixel.x];
        Ray ray = { m_ActiveCamera->GetPosition(), rayDirection };
        return TraceRay(ray).colour;
    }

    Payload Renderer::TraceRay(const Ray& ray)
    {
        Payload finalPayload = { { 0, 0, 0, 0 }, FLT_MAX };
        int32_t closestHitId = -1;
        for (int32_t sphereId = 0; sphereId < m_ActiveScene->Spheres.size(); sphereId++)
        {
            const Sphere& sphere = m_ActiveScene->Spheres[sphereId];

            glm::vec3 origin = ray.origin - sphere.position;

            float a = glm::dot(ray.direction, ray.direction);
            float b = 2 * glm::dot(origin, ray.direction);
            float c = glm::dot(origin, origin) - sphere.radius * sphere.radius;

            float delta = b * b - 4 * a * c;
            if (delta < 0)
            {
                continue;
            }

            float t = (-b - glm::sqrt(delta)) / (2.0f * a);
            if (t < 0)
            {
                continue;
            }
            if (finalPayload.hitDistance < t)
                continue;

            finalPayload.hitDistance = t;
            closestHitId = sphereId;
        }

        if (closestHitId == -1)
            return Miss(ray);

        return ClosestHit(ray, closestHitId);
    }

    Payload Renderer::ClosestHit(const Ray& ray, int32_t objectId)
    {
        const Sphere& sphere = m_ActiveScene->Spheres[objectId];

        glm::vec3 origin = ray.origin - sphere.position;

        float a = glm::dot(ray.direction, ray.direction);
        float b = 2 * glm::dot(origin, ray.direction);
        float c = glm::dot(origin, origin) - sphere.radius * sphere.radius;

        float delta = b * b - 4 * a * c;
        
        float t = (-b - glm::sqrt(delta)) / (2.0f * a);

        glm::vec3 hitPos = ray.origin + t * ray.direction;
        glm::vec3 sphereNormal = glm::normalize(hitPos - sphere.position);

        glm::vec3 lightDir = glm::normalize(glm::vec3(-1, -1, -1));
        float intensity = glm::clamp(glm::dot(sphereNormal, -lightDir), 0.0f, 1.0f);

        Payload payload;
        payload.colour = sphere.color * intensity;
        payload.hitDistance = t;
        payload.objectId = objectId;
        return payload;
    }

    Payload Renderer::Miss(const Ray& ray)
    {
        Payload payload;
        payload.colour = { 0, 0, 0, 1 };
        payload.hitDistance = FLT_MAX;
        payload.objectId = -1;
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
        return (float)seed / std::numeric_limits<float>::max();
    }

}
