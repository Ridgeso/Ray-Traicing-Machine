#include "Scene.h"

namespace RT::Render
{

    glm::vec3 GetEmmision(const Material& mat)
    {
        return mat.EmmisionColor * mat.EmmisionPower;
    }

}
