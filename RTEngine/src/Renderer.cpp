#include <glad/glad.h>

#include <fstream>
#include <sstream>
#include <algorithm>
#include <execution>
#include <cstddef>

#include "Renderer.h"

namespace RT::Render
{

    Renderer::Renderer()
        : Accumulate(true), m_FrameIndex(0)
        , m_SpecSize(0), m_RenderSize(0)
        , m_RenderId(0)
        , m_ScreenBuffer(0), m_FrameBufferId(0), m_RenderBuffer(0)
        , m_Program(0)
    {
    }

    bool Renderer::Invalidate(int32_t width, int32_t height)
    {
        m_SpecSize = { width, height };

        glCreateBuffers(1, &m_ScreenBuffer);
        glBindBuffer(GL_ARRAY_BUFFER, m_ScreenBuffer);
        glBufferData(GL_ARRAY_BUFFER, sizeof(s_Screen), s_Screen, GL_STATIC_DRAW);
        
        glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(Vertices), (void*)offsetof(Vertices, Coords));
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(Vertices), (void*)offsetof(Vertices, TexCoords));
        glEnableVertexAttribArray(0);
        glEnableVertexAttribArray(1);
        glBindBuffer(GL_ARRAY_BUFFER, 0);

        glCreateFramebuffers(1, &m_FrameBufferId);
        glBindFramebuffer(GL_FRAMEBUFFER, m_FrameBufferId);

        glActiveTexture(GL_TEXTURE0);
        glGenTextures(1, &m_RenderId);
        glBindTexture(GL_TEXTURE_2D, m_RenderId);
        
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, m_SpecSize.x, m_SpecSize.y, 0, GL_RGBA, GL_FLOAT, nullptr);
        
        glBindTexture(GL_TEXTURE_2D, 0);

        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_RenderId, 0);
        
        glGenRenderbuffers(1, &m_RenderBuffer);
        glBindRenderbuffer(GL_RENDERBUFFER, m_RenderBuffer);
        glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH32F_STENCIL8, m_SpecSize.x, m_SpecSize.y);
        glBindRenderbuffer(GL_RENDERBUFFER, 0);
        
        glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, m_RenderBuffer);
        
        if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
            return false;

        glBindFramebuffer(GL_FRAMEBUFFER, 0);

        std::ifstream shaders("../RTEngine/shaders/RayTracing.shader", std::ios::in);
        if (!shaders.is_open())
            return false;

        std::string line;
        enum { Vertex = 0, Fragment = 1 } shaderType;
        std::stringstream shadersSource[2];

        while (std::getline(shaders, line))
        {
            if (line.find("###SHADER") != std::string::npos)
            {
                if (line.find("(VERTEX)") != std::string::npos)
                    shaderType = Vertex;
                else if (line.find("(FRAGMENT)") != std::string::npos)
                    shaderType = Fragment;
            }
            else
                shadersSource[shaderType] << line << '\n';
        }

        uint32_t vertexShaderId = glCreateShader(GL_VERTEX_SHADER);
        uint32_t fragmentShaderId = glCreateShader(GL_FRAGMENT_SHADER);

        CompileShader(vertexShaderId, shadersSource[Vertex].str());
        CompileShader(fragmentShaderId, shadersSource[Fragment].str());
        m_Program = glCreateProgram();
        glAttachShader(m_Program, vertexShaderId);
        glAttachShader(m_Program, fragmentShaderId);
        glLinkProgram(m_Program);
        glValidateProgram(m_Program);

        glDeleteShader(vertexShaderId);
        glDeleteShader(fragmentShaderId);
        return true;
    }

    void Renderer::Devalidate()
    {
        glDeleteBuffers(1, &m_ScreenBuffer);
        glDeleteRenderbuffers(1, &m_RenderId);
        glDeleteRenderbuffers(1, &m_RenderBuffer);
        glDeleteFramebuffers(1, &m_FrameBufferId);
    }

    void Renderer::OnResize(int32_t width, int32_t height)
    {
        if (m_RenderSize.x != width || m_RenderSize.y != height)
        {
            m_RenderSize = { width, height };
            ResetFrame();
        }
    }

    bool Renderer::RecreateRenderer(int32_t width, int32_t height)
    {
        if (m_SpecSize.x != width || m_SpecSize.y != height)
        {
            Devalidate();
            return Invalidate(width, height);
        }
        return true;
    }

    void Renderer::Render(const Camera& camera, const Scene& scene)
    {
        glBindFramebuffer(GL_FRAMEBUFFER, m_FrameBufferId);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glEnable(GL_DEPTH_TEST);

        glUseProgram(m_Program);
        glBindBuffer(GL_ARRAY_BUFFER, m_ScreenBuffer);
        glBindTexture(GL_TEXTURE_2D, m_RenderId);
        glDrawArrays(GL_TRIANGLES, 0, 24);
        
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindTexture(GL_TEXTURE_2D, 0);
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }

    void Renderer::CompileShader(uint32_t shaderID, const std::string& source) const
    {
        const char* vertexSourceData = source.c_str();
        int32_t vertexSourceLenght = source.length();
        glShaderSource(shaderID, 1,
            &vertexSourceData,
            &vertexSourceLenght
        );
        glCompileShader(shaderID);

        //int32_t status;
        //glGetShaderiv(shaderID, GL_COMPILE_STATUS, &status);
        //if (status == GL_FALSE)
        //{
        //    int32_t lenght;
        //    glGetShaderiv(shaderID, GL_INFO_LOG_LENGTH, &lenght);
        //    std::vector<char> message(lenght);
        //    glGetShaderInfoLog(shaderID, lenght, &lenght, message.data());
        //    std::cout << "SHADER ERROR: " << message.data() << std::endl;
        //}
    }

}
