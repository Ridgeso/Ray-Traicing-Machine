#include <fstream>
#include <sstream>
#include <iostream>
#include <algorithm>
#include <execution>
#include <cstddef>

#include <glad/glad.h>
#include <glm/gtc/type_ptr.hpp>
#include <GLFW/glfw3.h>

#include "OpenGlRenderer.h"

namespace RT
{

    void OpenGlRenderer::init(const RenderSpecs& specs)
    {
        loadOpenGlForGlfw();

        resolutionUni.value = { specs.width, specs.height };
        accumulation = specs.accumulate;

        resize(resolutionUni.value);

        std::ifstream shaders("..\\Engine\\assets\\shaders\\RayTracing.shader", std::ios::in);
        if (!shaders.is_open())
            return;

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

        compileShader(vertexShaderId, shadersSource[Vertex].str());
        compileShader(fragmentShaderId, shadersSource[Fragment].str());
        programId = glCreateProgram();
        glAttachShader(programId, vertexShaderId);
        glAttachShader(programId, fragmentShaderId);
        glLinkProgram(programId);
        glValidateProgram(programId);

        glDeleteShader(vertexShaderId);
        glDeleteShader(fragmentShaderId);

        accumulationSamplerUni.ID = glGetUniformLocation(programId, accumulationSamplerUni.name.c_str());
        renderSamplerUni.ID = glGetUniformLocation(programId, renderSamplerUni.name.c_str());
        drawEnvironmentUni.ID = glGetUniformLocation(programId, drawEnvironmentUni.name.c_str());
        maxBouncesUni.ID = glGetUniformLocation(programId, maxBouncesUni.name.c_str());
        maxFramesUni.ID = glGetUniformLocation(programId, maxFramesUni.name.c_str());
        frameIndexUni.ID = glGetUniformLocation(programId, frameIndexUni.name.c_str());
        resolutionUni.ID = glGetUniformLocation(programId, resolutionUni.name.c_str());
        materialsCountUni.ID = glGetUniformLocation(programId, materialsCountUni.name.c_str());
        spheresCountUni.ID = glGetUniformLocation(programId, spheresCountUni.name.c_str());

        glGenBuffers(1, &cameraStorage);
        glGenBuffers(1, &materialsStorage);
        glGenBuffers(1, &spheresStorage);
    }

    void OpenGlRenderer::shutDown()
    {
        glDeleteBuffers(1, &cameraStorage);
        glDeleteBuffers(1, &materialsStorage);
        glDeleteBuffers(1, &spheresStorage);

        glDeleteTextures(1, &accumulationId);
        glDeleteTextures(1, &renderId);
        glDeleteProgram(programId);
        glDeleteBuffers(1, &screenBufferId);
        glDeleteRenderbuffers(1, &renderBufferId);
        glDeleteFramebuffers(1, &frameBufferId);
    }

    bool OpenGlRenderer::recreateRenderer(const glm::ivec2 size)
    {
        if (resolutionUni.value == size)
            return true;
        
        resetFrame();
        shutDown();
        resize(size);
        
        return false;
    }

    void OpenGlRenderer::render(const Camera& camera, const Scene& scene)
    {
        frameIndexUni.value++;
        if (!accumulation)
        {
            frameIndexUni.value = 1;
        }

        glBindFramebuffer(GL_FRAMEBUFFER, frameBufferId);

        glUseProgram(programId);

        glUniform1i(accumulationSamplerUni.ID, accumulationSamplerUni.value);
        glUniform1i(renderSamplerUni.ID, renderSamplerUni.value);
        glUniform1f(drawEnvironmentUni.ID, (float)drawEnvironmentUni.value);
        glUniform1ui(maxBouncesUni.ID, maxBouncesUni.value);
        glUniform1ui(maxFramesUni.ID, maxFramesUni.value);
        glUniform1ui(frameIndexUni.ID, frameIndexUni.value);
        glUniform2f(resolutionUni.ID, (float)resolutionUni.value.x, (float)resolutionUni.value.y);

        glBindBuffer(GL_SHADER_STORAGE_BUFFER, cameraStorage);
        glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(Camera::Spec), &camera.GetSpec(), GL_DYNAMIC_DRAW);
        glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, cameraStorage);
        
        glUniform1i(materialsCountUni.ID, scene.materials.size());
        glBindBuffer(GL_SHADER_STORAGE_BUFFER, materialsStorage);
        glBufferData(
            GL_SHADER_STORAGE_BUFFER,
            sizeof(Material) * scene.materials.size(),
            scene.materials.data(),
            GL_DYNAMIC_DRAW
        );
        glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, materialsStorage);
        
        glUniform1i(spheresCountUni.ID, scene.spheres.size());
        glBindBuffer(GL_SHADER_STORAGE_BUFFER, spheresStorage);
        glBufferData(GL_SHADER_STORAGE_BUFFER,
            sizeof(Sphere) * scene.spheres.size(),
            scene.spheres.data(),
            GL_DYNAMIC_DRAW
        );
        glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 2, spheresStorage);
        glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);

        glBindBuffer(GL_ARRAY_BUFFER, screenBufferId);

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, accumulationId);
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, renderId);
        
        glDrawArrays(GL_TRIANGLES, 0, sizeof(s_Screen) / sizeof(float));
        
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, 0);
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, 0);

        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }

    void OpenGlRenderer::resize(const glm::ivec2 size)
    {
        resolutionUni.value = size;
        glViewport(0, 0, size.x, size.y);

        glCreateBuffers(1, &screenBufferId);
        glBindBuffer(GL_ARRAY_BUFFER, screenBufferId);
        glBufferData(GL_ARRAY_BUFFER, sizeof(s_Screen), s_Screen, GL_STATIC_DRAW);

        glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(Vertices), (void*)offsetof(Vertices, Coords));
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(Vertices), (void*)offsetof(Vertices, TexCoords));
        glEnableVertexAttribArray(0);
        glEnableVertexAttribArray(1);
        glBindBuffer(GL_ARRAY_BUFFER, 0);

        glCreateFramebuffers(1, &frameBufferId);
        glBindFramebuffer(GL_FRAMEBUFFER, frameBufferId);

        #define AttachColorTexture(TexId, Format, Width, Height)                                  \
            glGenTextures(1, &TexId);                                                             \
            glBindTexture(GL_TEXTURE_2D, TexId);                                                  \
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);                    \
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);                    \
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);                  \
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);                  \
            glTexImage2D(GL_TEXTURE_2D, 0, Format, Width, Height, 0, GL_RGBA, GL_FLOAT, nullptr); \
            glBindTexture(GL_TEXTURE_2D, 0)

        AttachColorTexture(accumulationId, GL_RGBA32F, resolutionUni.value.x, resolutionUni.value.y);
        AttachColorTexture(renderId, GL_RGBA32F, resolutionUni.value.x, resolutionUni.value.y);

        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, accumulationId, 0);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, renderId, 0);

        glGenRenderbuffers(1, &renderBufferId);
        glBindRenderbuffer(GL_RENDERBUFFER, renderBufferId);
        glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, resolutionUni.value.x, resolutionUni.value.y);
        glBindRenderbuffer(GL_RENDERBUFFER, 0);

        glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, renderBufferId);

        constexpr uint32_t buffers[] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1 };
        glDrawBuffers(2, buffers);

        if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
            return;

        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }

    void OpenGlRenderer::compileShader(uint32_t shaderID, const std::string& source) const
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

    void OpenGlRenderer::loadOpenGlForGlfw()
    {
        gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);
    }

}
