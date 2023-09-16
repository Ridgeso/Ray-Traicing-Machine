#include <glad/glad.h>
#include <glm/gtc/type_ptr.hpp>

#include <fstream>
#include <sstream>
#include <iostream>
#include <algorithm>
#include <execution>
#include <cstddef>

#include "Renderer.h"

namespace RT::Render
{

    Renderer::Renderer()
        : DrawEnvironment(false), Accumulate(true), MaxBounces(5), MaxFrames(1), m_FrameIndex(0)
        , m_SpecSize(0), m_RenderSize(0)
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

        #define AttachColorTexture(TexId, Format, Width, Height) \
            glGenTextures(1, &TexId); \
            glBindTexture(GL_TEXTURE_2D, TexId); \
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST); \
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST); \
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE); \
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE); \
            glTexImage2D(GL_TEXTURE_2D, 0, Format, Width, Height, 0, GL_RGBA, GL_FLOAT, nullptr); \
            glBindTexture(GL_TEXTURE_2D, 0)

        AttachColorTexture(m_AccumulationId, GL_RGBA32F, m_SpecSize.x, m_SpecSize.y);
        AttachColorTexture(m_RenderId, GL_RGBA32F, m_SpecSize.x, m_SpecSize.y);

        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_AccumulationId, 0);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, m_RenderId, 0);

        glGenRenderbuffers(1, &m_RenderBuffer);
        glBindRenderbuffer(GL_RENDERBUFFER, m_RenderBuffer);
        glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, m_SpecSize.x, m_SpecSize.y);
        glBindRenderbuffer(GL_RENDERBUFFER, 0);
        
        glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, m_RenderBuffer);

        constexpr uint32_t buffers[] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1 };
        glDrawBuffers(2, buffers);

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

        u_AccumulationTexture = glGetUniformLocation(m_Program, "u_AccumulationTexture");
        u_ScreenTexture = glGetUniformLocation(m_Program, "u_ScreenTexture");
        u_MaxBounces = glGetUniformLocation(m_Program, "u_MaxBounces");
        u_DrawEnvironment = glGetUniformLocation(m_Program, "u_DrawEnvironment");
        u_MaxFrames = glGetUniformLocation(m_Program, "u_MaxFrames");
        u_FrameIndex = glGetUniformLocation(m_Program, "u_FrameIndex");
        u_Resolution = glGetUniformLocation(m_Program, "u_Resolution");
        u_MaterialsCount = glGetUniformLocation(m_Program, "u_MaterialsCount");
        u_SpheresCount = glGetUniformLocation(m_Program, "u_SpheresCount");

        glGenBuffers(1, &u_CameraStorage);
        glGenBuffers(1, &u_MaterialsStorage);
        glGenBuffers(1, &u_SpheresStorage);

        return true;
    }

    void Renderer::Devalidate()
    {
        glDeleteBuffers(1, &u_CameraStorage);
        glDeleteBuffers(1, &u_MaterialsStorage);
        glDeleteBuffers(1, &u_SpheresStorage);

        glDeleteTextures(1, &m_AccumulationId);
        glDeleteTextures(1, &m_RenderId);
        glDeleteProgram(m_Program);
        glDeleteBuffers(1, &m_ScreenBuffer);
        glDeleteRenderbuffers(1, &m_RenderBuffer);
        glDeleteFramebuffers(1, &m_FrameBufferId);
    }

    bool Renderer::RecreateRenderer(int32_t width, int32_t height)
    {
        if (m_SpecSize != glm::ivec2(width, height))
        {
            ResetFrame();
            Devalidate();
            return Invalidate(width, height);
        }
        return true;
    }

    void Renderer::Render(const Camera& camera, const Scene& scene)
    {
        m_FrameIndex++;
        if (!Accumulate)
            m_FrameIndex = 1;

        glBindFramebuffer(GL_FRAMEBUFFER, m_FrameBufferId);

        glUseProgram(m_Program);
        glUniform1i(u_AccumulationTexture, 0);
        glUniform1i(u_ScreenTexture, 1);
        glUniform1f(u_DrawEnvironment, (float)DrawEnvironment);
        glUniform1ui(u_MaxBounces, MaxBounces);
        glUniform1ui(u_MaxFrames, MaxFrames);
        glUniform1ui(u_FrameIndex, m_FrameIndex);
        glUniform2f(u_Resolution, (float)m_SpecSize.x, (float)m_SpecSize.y);

        glBindBuffer(GL_SHADER_STORAGE_BUFFER, u_CameraStorage);
        glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(Camera::Spec), &camera.GetSpec(), GL_DYNAMIC_DRAW);
        glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, u_CameraStorage);
        
        glUniform1i(u_MaterialsCount, scene.Materials.size());
        glBindBuffer(GL_SHADER_STORAGE_BUFFER, u_MaterialsStorage);
        glBufferData(GL_SHADER_STORAGE_BUFFER,
            sizeof(Material) * scene.Materials.size(),
            scene.Materials.data(),
            GL_DYNAMIC_DRAW
        );
        glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, u_MaterialsStorage);
        
        glUniform1i(u_SpheresCount, scene.Spheres.size());
        glBindBuffer(GL_SHADER_STORAGE_BUFFER, u_SpheresStorage);
        glBufferData(GL_SHADER_STORAGE_BUFFER,
            sizeof(Sphere) * scene.Spheres.size(),
            scene.Spheres.data(),
            GL_DYNAMIC_DRAW
        );
        glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 2, u_SpheresStorage);
        glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);

        glBindBuffer(GL_ARRAY_BUFFER, m_ScreenBuffer);

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, m_AccumulationId);
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, m_RenderId);
        
        glDrawArrays(GL_TRIANGLES, 0, sizeof(s_Screen) / sizeof(float));
        
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, 0);
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, 0);

        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }

    void Renderer::CompileShader(uint32_t shaderID, const std::string& source) const
    {
        const char* sourceData = source.c_str();
        int32_t sourceLenght = source.length();
        glShaderSource(shaderID, 1,
            &sourceData,
            &sourceLenght
        );
        glCompileShader(shaderID);

        int32_t status;
        glGetShaderiv(shaderID, GL_COMPILE_STATUS, &status);
        if (status == GL_FALSE)
        {
            int32_t lenght;
            glGetShaderiv(shaderID, GL_INFO_LOG_LENGTH, &lenght);
            std::vector<char> message(lenght);
            glGetShaderInfoLog(shaderID, lenght, &lenght, message.data());
            std::cout << "SHADER ERROR:\n" << message.data() << std::endl;
        }
    }

}
