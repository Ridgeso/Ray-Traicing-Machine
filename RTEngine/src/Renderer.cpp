#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

#include "Renderer.h"

namespace RT::Render
{
    
    Renderer::Renderer()
        : m_FrameIndex(0), m_Seed(0), m_MainView(), m_ViewCash(nullptr)
    {
        m_MainShpere.position = { 0.f, 0.f, 10.f };
        m_MainShpere.radius = 0.5f;
    }

    void Renderer::Render(const Scene& scene)
    {
        float uv = (float)m_MainView.GetWidth() / m_MainView.GetHeight();

        m_FrameIndex += 1;
        m_Seed += m_FrameIndex;
        for (int32_t y = 0; y < m_MainView.GetHeight(); y++)
        {
            for (int32_t x = 0; x < m_MainView.GetWidth(); x++)
            {
                glm::vec3 coord = 2.0f * glm::vec3((float)x / m_MainView.GetWidth(), (float)y / m_MainView.GetHeight(), -1.0) - 1.0f;
                coord.x *= uv;
                m_ViewCash[y * m_MainView.GetWidth() + x] = PixelColor(coord, scene.Spheres[0]);
            }
        }
        m_MainView.Update(m_ViewCash);
    }

    void Renderer::OnResize(int32_t width, int32_t height)
    {
        if (m_MainView.GetWidth() != width || m_MainView.GetHeight() != height)
        {
            m_FrameIndex = 0;
            m_MainView.Resize(width, height);
            delete[] m_ViewCash;
            m_ViewCash = new uint32_t[m_MainView.GetWidth() * m_MainView.GetHeight()];
        }
    }

    uint32_t Renderer::PixelColor(glm::vec3 coord, const Sphere& sphere)
    {
        glm::vec3 pos = { 0, 0, 2 };
        glm::vec3 origin = pos - sphere.position;

        float a = glm::dot(coord, coord);
        float b = 2 * glm::dot(origin, coord);
        float c = glm::dot(origin, origin) - sphere.radius * sphere.radius;

        float delta = b * b - 4 * a * c;

        if (delta >= 0)
        {
            uint8_t r = (uint8_t)(sphere.color.x * 255.f);
            uint8_t g = (uint8_t)(sphere.color.y * 255.f);
            uint8_t b = (uint8_t)(sphere.color.z * 255.f);
            return 0xff000000 | (b << 16) | (g << 8) | r;
        }

        return 0xff000000;
    }

    static inline uint32_t pcg_hash(uint32_t input)
    {
        uint32_t state = input * 747796405u + 2891336453u;
        uint32_t word = ((state >> ((state >> 28u) + 4u)) ^ state) * 277803737u;
        return (word >> 22u) ^ word;
    }

    uint32_t FastRandom(uint32_t& seed)
    {
        seed = pcg_hash(seed);
        return seed;
    }

}
