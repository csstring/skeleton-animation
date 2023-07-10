#include "include/AnimationDataResize.h"
#include "include/Animation.h"

void AnimationDataResize::processNode(AnimationData& data)
{
    glm::mat4 mat(1.0f);
    data._localRotation.resize(_size);
    data._worldTrans.resize(_size,mat);
    data._localTrans.resize(_size, glm::vec4(0.0f));
}