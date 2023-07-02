#include "AnimationDataMatrixSetup.h"
#include "Animation.h"

#include "include/GLM/glm.hpp"
#include "include/GLM/gtc/matrix_inverse.hpp"
void AnimationDataMatrixSetup::processNode(AnimationData& data)
{
    glm::mat4 invC = glm::inverse(data.__c);
    for (int i =0; i <data._matrix.size(); ++i)
        data._matrix[i] = data.__b * data.__c * data._matrix[i] * invC;

    if (data._name != "root")
    {
        for (int i =0; i <data._matrix.size(); ++i)
        {
            data._matrix[i] = data._parentPointer->_matrix[i] * data._matrix[i];
        }
    }
}