#include "Renderer.h"

namespace RT::Render
{
    
    Renderer::Renderer()
        : m_Size{ 0 }, m_Image{ 0 }
    {
    }

    void Renderer::Render()
    {
        for (int32_t y = 0; y < m_Size.y; y++)
            for (int32_t x = 0; x < m_Size.x; x++)
            {
                m_Image[y * m_Size.x + x] = 0xff00ffff;
            }
    }

    void Renderer::OnResize(const int32_t width, const int32_t height)
    {
        if (m_Size.x != width || m_Size.y != height)
        {
            m_Size = { width, height };
            m_Image.resize(m_Size.x * m_Size.y);
        }
    }

}
