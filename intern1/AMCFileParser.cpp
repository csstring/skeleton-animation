#include "include/AMCFileParser.h"
#include "include/Skeleton.h"
#include "include/Animation.h"
#include "include/Bone.h"
#include "AnimationDataResize.h"
#include "AnimationDataMatrixSetup.h"
#include "include/GLM/glm.hpp"
#include "include/GLM/gtc/matrix_transform.hpp"
#include "fstream"
#include <algorithm>
#include <iostream>
#include <cctype>

bool AMCFileParser::loadAMCFile(void)
{
    std::ifstream ifs(_filePath);
    std::string buffer;

    if (ifs.is_open() == false)
        return false;
    while (std::getline(ifs,buffer))
    {
        if ('0' <= buffer[0] && buffer[0] <= '9')
            _total++;
    }
    ifs.close();

    AnimationDataResize dataResizer(_total);
    _animation->AnimationDataTraver(dataResizer);

    ifs.open(_filePath);
    while (std::getline(ifs,buffer))
    {
        if (buffer[0] !='#' && buffer[0] != ':')
            break;
    }

    int32 Index = 0;

    while (ifs >> buffer)
    {
        AnimationData* animationData = _animation->returnAnimationData(buffer);

        if (animationData == NULL)
        {
            Index = std::stoi(buffer);
            continue;
        }
        
        std::vector<DOF> dofs = _skeleton->getBoneVector()[animationData->_boneIndex]._dof;
        for (DOF dof : dofs)
        {
            float val;
            ifs >> val;

            if (dof == DOF::RX)
                animationData->_matrix[Index] = glm::rotate(animationData->_matrix[Index], glm::radians(val), glm::vec3(1.0f,0.0f,0.0f));   
            else if (dof == DOF::RY)
                animationData->_matrix[Index] = glm::rotate(animationData->_matrix[Index], glm::radians(val), glm::vec3(0.0f,1.0f,0.0f)); 
            else if (dof == DOF::RZ)
                animationData->_matrix[Index] = glm::rotate(animationData->_matrix[Index], glm::radians(val), glm::vec3(0.0f,0.0f,1.0f)); 
            else if (dof == DOF::TX)
                animationData->_matrix[Index] = glm::translate(animationData->_matrix[Index], glm::vec3(val, 0.0f, 0.0f));
            else if (dof == DOF::TY)
                animationData->_matrix[Index] = glm::translate(animationData->_matrix[Index], glm::vec3(0.0f, val, 0.0f));
            else if (dof == DOF::TZ)
                animationData->_matrix[Index] = glm::translate(animationData->_matrix[Index], glm::vec3(0.0f, 0.0f, val));
        }
    }
}