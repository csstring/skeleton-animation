#include "include/AnimationDataMatrixSetup.h"
#include "include/Animation.h"

#include "include/GLM/glm.hpp"
#include "include/GLM/gtc/matrix_inverse.hpp"
void AnimationDataMatrixSetup::processNode(AnimationData& data)
{
    if (data._name == "root")
    {
        for (int i =0; i <data._matrix.size(); ++i)
        {
            //data._pos[i] = glm::vec4(0.0f);
            //data._pos[i] = data._matrix[i] * data._pos[i];
            //data._matrix[i] = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f));
            data._pos[i] = data._matrix[i] * glm::vec4(0.0f,0.0f,0.0f,1.0f);
        }
        return;
    }
    
    glm::mat4 invC = glm::inverse(data.__c);
    for (int i =0; i <data._matrix.size(); ++i)
    {
    
        glm::mat4 rotatinM = data.__c * data._matrix[i] * invC;
        glm::vec3 transV = rotatinM * data.__b;//vec4
        glm::mat4 trnasM = glm::translate(glm::mat4(1.0f), transV);
        
        data._matrix[i] = data._parentPointer->_matrix[i] * trnasM * rotatinM;
        data._pos[i] = data._matrix[i] * glm::vec4(0.0f,0.0f,0.0f,1.0f);
/*
        glm::mat4 trnasM = data.__b * data.__c * data._matrix[i] * invC;
        glm::mat4 rotatinM = data.__c * data._matrix[i] * invC;
        
        data._matrix[i] = data._parentPointer->_matrix[i] * trnasM * rotatinM;
        data._pos[i] = data._matrix[i] * glm::vec4(0.0f,0.0f,0.0f,1.0f);
        ##########이렇게 하면 안됨
*/
    }
}