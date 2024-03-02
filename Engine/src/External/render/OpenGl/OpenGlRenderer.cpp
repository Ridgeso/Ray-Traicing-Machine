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

        screenBuff = makeLocal<OpenGlVertexBuffer>(static_cast<uint32_t>(sizeof(screenVertices)));
        screenBuff->setData(sizeof(screenVertices), screenVertices);
        screenBuff->registerAttributes({ VertexElement::Float2, VertexElement::Float2 });

        frameBuffer = makeLocal<OpenGlFrameBuffer>(resolutionUni.value, 2);
    }

    void OpenGlRenderer::shutDown()
    {
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
        
        frameBuffer->bind();
        screenBuff->bind();

        frameBuffer->getAttachment(0).bind(0);
        frameBuffer->getAttachment(1).bind(1);

        glDrawArrays(GL_TRIANGLES, 0, screenVerticesCount);

        screenBuff->unbind();
        frameBuffer->unbind();

        openGlShader.unuse();
    }

    void OpenGlRenderer::resize(const glm::ivec2 size)
    {
        resolutionUni.value = size;
        frameBuffer = makeLocal<OpenGlFrameBuffer>(resolutionUni.value, 2);
    }

    void OpenGlRenderer::loadOpenGlForGlfw()
    {
        gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);
    }

}
