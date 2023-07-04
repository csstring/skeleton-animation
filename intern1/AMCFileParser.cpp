#include "include/AMCFileParser.h"
#include "include/Skeleton.h"
#include "include/Animation.h"
#include "include/Bone.h"
#include "include/AnimationDataResize.h"
#include "include/AnimationDataMatrixSetup.h"
#include "include/GLM/glm.hpp"
#include "include/GLM/gtx/transform.hpp"
#include "include/GLM/gtc/matrix_transform.hpp"
#include "fstream"
#include <algorithm>
#include <iostream>
#include <cctype>

void AMCFileParser::boneDataindexing(std::vector<AnimationData*>& indexVector)
{
    std::ifstream ifs(_filePath);
    std::string buffer;
    uint32 maxBone = _skeleton->getBoneVector().size();
    uint32 count = 0;

    while (std::getline(ifs,buffer))
    {
        if (buffer[0] !='#' && buffer[0] != ':')
            break;
    }
    while (count < maxBone)
    {
        std::getline(ifs,buffer,' ');
        indexVector.push_back(_animation->returnAnimationData(buffer));
        std::getline(ifs,buffer);
        count++;
    }
    ifs.close();
}

//캐싱, resize, capa확보후 pushback
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

    std::vector<AnimationData*> indexVector;
    boneDataindexing(indexVector);

    AnimationDataResize dataResizer(_total);
    _animation->AnimationDataTraver(dataResizer);

    ifs.open(_filePath);
    while (std::getline(ifs,buffer))
    {
        if (buffer[0] !='#' && buffer[0] != ':')
            break;
    }

    uint32 Index = 0;
    uint32 count = 0;
    while (ifs >> buffer)
    {
        
        if (count == indexVector.size())
        {
            count = 0;
            Index = std::stoi(buffer)-1;
            continue;
        }

        AnimationData* animationData = indexVector[count];
        std::vector<DOF> dofs = _skeleton->getBoneVector()[animationData->_boneIndex]._dof;
        for (DOF dof : dofs)
        {
            float val;
            ifs >> val;

            if (dof == DOF::RX)
                animationData->_matrix[Index] = glm::rotate(glm::radians(val), glm::vec3(1.0f,0.0f,0.0f)) * animationData->_matrix[Index];   
            else if (dof == DOF::RY)
                animationData->_matrix[Index] = glm::rotate(glm::radians(val), glm::vec3(0.0f,1.0f,0.0f)) * animationData->_matrix[Index]; 
            else if (dof == DOF::RZ)
                animationData->_matrix[Index] = glm::rotate(glm::radians(val), glm::vec3(0.0f,0.0f,1.0f)) * animationData->_matrix[Index]; 
            else if (dof == DOF::TX)
                animationData->_matrix[Index] = glm::translate(glm::vec3(val, 0.0f, 0.0f)) * animationData->_matrix[Index];
            else if (dof == DOF::TY)
                animationData->_matrix[Index] = glm::translate(glm::vec3(0.0f, val, 0.0f)) * animationData->_matrix[Index];
            else if (dof == DOF::TZ)
                animationData->_matrix[Index] = glm::translate(glm::vec3(0.0f, 0.0f, val)) * animationData->_matrix[Index];
        }
    }
    return true;
}