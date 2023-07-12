#include "include/AnimationDataResize.h"
#include "include/Animation.h"

void AnimationDataResize::processNode(AnimationData& data)
{
    glm::mat4 mat(1.0f);
    data._localRotation.reserve(_size);
    data._localTrans.reserve(_size);
}