#include "include/AnimationDataResize.h"
#include "include/Animation.h"

void AnimationDataResize::processNode(AnimationData& data)
{
    glm::mat4 mat(1.0f);
    data._matrix.resize(_size, mat);
    data._pos.resize(_size, glm::vec4(0.0f,0.0f,0.0f,1.0f));
}