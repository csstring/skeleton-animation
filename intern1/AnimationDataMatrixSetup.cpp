#include "include/AnimationDataMatrixSetup.h"
#include "include/Animation.h"
#include "include/GLM/gtx/quaternion.hpp"
#include "include/GLM/gtc/matrix_inverse.hpp"
#include <deque>
void AnimationDataMatrixSetup::processNode(AnimationData& data)
{
    for (int i =0; i <data._localRotation.size(); ++i)
    {
        if (data._parentPointer != NULL)
            data._worldTrans[i] = data._parentPointer->_worldTrans[i] * data._localTrans[i] * glm::toMat4(data._localRotation[i]);
        else
            data._worldTrans[i] = data._localTrans[i] * glm::toMat4(data._localRotation[i]);
    }
}