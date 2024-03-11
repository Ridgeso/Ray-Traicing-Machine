#include "Shader.h"
#include "External/Render/OpenGl/OpenGlShader.h"

namespace RT
{

    Local<Shader> RT::createShader()
    {
        return makeLocal<OpenGl::OpenGlShader>();
    }

}
