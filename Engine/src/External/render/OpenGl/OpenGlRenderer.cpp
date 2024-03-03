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

        this->specs = specs;

        frameBuffer = makeLocal<OpenGlFrameBuffer>(specs.size, 2);
    }

    void OpenGlRenderer::shutDown()
    {
    }

    bool OpenGlRenderer::recreateRenderer(const glm::ivec2 size)
    {
        if (specs.size == size)
            return true;
        
        resize(size);
        return false;
    }

    void OpenGlRenderer::render(const Camera& camera, const Shader& shader, const VertexBuffer& vbuffer, const Scene& scene)
    {
        const auto& openGlShader = static_cast<const OpenGlShader&>(shader);
        const auto& openGlVbuffer = static_cast<const OpenGlVertexBuffer&>(vbuffer);

        openGlShader.use();
        frameBuffer->bind();
        openGlVbuffer.bind();

        frameBuffer->getAttachment(0).bind(0);
        frameBuffer->getAttachment(1).bind(1);

        glDrawArrays(GL_TRIANGLES, 0, openGlVbuffer.getCount());

        openGlVbuffer.unbind();
        frameBuffer->unbind();
        openGlShader.unuse();
    }

    void OpenGlRenderer::resize(const glm::ivec2 size)
    {
        this->specs.size = size;
        frameBuffer = makeLocal<OpenGlFrameBuffer>(specs.size, 2);
    }

    void OpenGlRenderer::loadOpenGlForGlfw()
    {
        gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);
    }

}
