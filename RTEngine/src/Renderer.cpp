#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "Renderer.h"

namespace RT::Render
{
    
    Renderer::Renderer()
        : m_FrameIndex(0), m_Seed(0), m_MainView(0, 0, nullptr)
    {
    }

    void Renderer::Render()
    {
        m_FrameIndex += 1;
        m_Seed += m_FrameIndex;
        uint32_t* processImage = new uint32_t[m_MainView.GetWidth() * m_MainView.GetHeight()];
        for (int32_t y = 0; y < m_MainView.GetHeight(); y++)
            for (int32_t x = 0; x < m_MainView.GetWidth(); x++)
            {
                processImage[y * m_MainView.GetWidth() + x] = FastRandom(m_Seed);
            }
        m_MainView.Update(processImage);
        delete[] processImage;
    }

    void Renderer::OnResize(const int32_t width, const int32_t height)
    {
        if (m_MainView.GetWidth() != width || m_MainView.GetHeight() != height)
        {
            m_FrameIndex = 0;
            m_MainView.Resize(width, height);
        }
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
