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

    void Renderer::Render()
    {
        m_FrameIndex += 1;
        m_Seed += m_FrameIndex;
        for (int32_t y = 0; y < m_MainView.GetHeight(); y++)
        {
            for (int32_t x = 0; x < m_MainView.GetWidth(); x++)
            {
                glm::vec2 coord = { (float)x / m_MainView.GetWidth(), (float)y / m_MainView.GetHeight() };
                m_ViewCash[y * m_MainView.GetWidth() + x] = PixelColor(coord);
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

    uint32_t Renderer::PixelColor(glm::vec2 coord)
    {
        uint8_t r = (uint8_t)(coord.x * 255.f);
        uint8_t g = (uint8_t)(coord.y * 255.f);

        return 0xff000000 | (g << 8) | r;
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
