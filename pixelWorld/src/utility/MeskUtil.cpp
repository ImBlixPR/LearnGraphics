#include "MeskUtil.h"

namespace MiskUtil
{
    glm::vec3 MiskUtil::GetNormalizeRGBColor(float r, float g, float b)
    {
        return glm::vec3( r/256, g/256, b/256);
    }
    
}
