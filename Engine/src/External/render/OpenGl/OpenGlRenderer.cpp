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

namespace RT::OpenGl
{

    void OpenGlRenderer::init(const RenderSpecs& specs)
    {
        loadOpenGlForGlfw();

        resolutionUni.value = { specs.width, specs.height };
        accumulation = specs.accumulate;

        resize(resolutionUni.value);
    }

    void OpenGlRenderer::shutDown()
    {
        clear();
    }

    bool OpenGlRenderer::recreateRenderer(const glm::ivec2 size)
    {
        if (resolutionUni.value == size)
            return true;
        
        resetFrame();
        resize(size);
        return false;
    }

    void OpenGlRenderer::render(const Camera& camera, const Shader& shader, const Scene& scene)
    {
        const auto& openGlShader = static_cast<const OpenGlShader&>(shader);

        frameIndexUni.value++;
        if (!accumulation)
        {
            frameIndexUni.value = 1;
        }

        openGlShader.use();
        openGlShader.setUniform(accumulationSamplerUni.name, 1, accumulationSamplerUni.value);
        openGlShader.setUniform(renderSamplerUni.name, 1, renderSamplerUni.value);
        openGlShader.setUniform(drawEnvironmentUni.name, 1, (float)drawEnvironmentUni.value);
        openGlShader.setUniform(maxBouncesUni.name, 1, maxBouncesUni.value);
        openGlShader.setUniform(maxFramesUni.name, 1, maxFramesUni.value);
        openGlShader.setUniform(frameIndexUni.name, 1, frameIndexUni.value);
        openGlShader.setUniform(resolutionUni.name, 1, (glm::vec2)resolutionUni.value);
        openGlShader.setUniform(cameraStorage.name, sizeof(Camera::Spec), camera.GetSpec());
        openGlShader.setUniform(materialsCountUni.name, 1, scene.materials.size());
        openGlShader.setUniform(materialsStorage.name, sizeof(Material) * scene.materials.size(), scene.materials.data());
        openGlShader.setUniform(spheresCountUni.name, 1, scene.spheres.size());
        openGlShader.setUniform(spheresStorage.name, sizeof(Sphere) * scene.spheres.size(), scene.spheres.data());
        
        glBindFramebuffer(GL_FRAMEBUFFER, frameBufferId);
        glBindBuffer(GL_ARRAY_BUFFER, screenBufferId);

        accumulationTex->bind(0);
        renderTex->bind(1);

        glDrawArrays(GL_TRIANGLES, 0, sizeof(s_Screen) / sizeof(float));

        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindFramebuffer(GL_FRAMEBUFFER, 0);

        openGlShader.unuse();
    }

    void OpenGlRenderer::clear()
    {
        glDeleteBuffers(1, &screenBufferId);
        glDeleteRenderbuffers(1, &renderBufferId);
        glDeleteFramebuffers(1, &frameBufferId);
    }

    void OpenGlRenderer::resize(const glm::ivec2 size)
    {
        clear();
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

        accumulationTex = makeLocal<OpenGlTexture>(resolutionUni.value, ImageFormat::RGBA32F);
        renderTex = makeLocal<OpenGlTexture>(resolutionUni.value, ImageFormat::RGBA32F);

        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, accumulationTex->getTexId(), 0);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, renderTex->getTexId(), 0);

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

    void OpenGlRenderer::loadOpenGlForGlfw()
    {
        gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);
    }

}
