#include "include/AnimationDataMatrixSetup.h"
#include "include/Animation.h"
#include "include/GLM/gtx/quaternion.hpp"
#include "include/GLM/gtc/matrix_inverse.hpp"
#include <deque>
void AnimationDataMatrixSetup::processNode(AnimationData& data)
{
    for (int i =0; i <data._localRotation.size(); ++i)
    {
        glm::mat4 rotatinM = glm::toMat4(data._c * data._localRotation[i] * data._invC);
        glm::vec3 transV = rotatinM * data._b + data._localTrans[i];
        glm::mat4 trnasM = glm::translate(glm::mat4(1.0f), transV);

        if (data._parentPointer != NULL)
            data._worldTrans[i] = data._parentPointer->_worldTrans[i] * trnasM * rotatinM;
        else
            data._worldTrans[i] = trnasM * rotatinM;
    }
}