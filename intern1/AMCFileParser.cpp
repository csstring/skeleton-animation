#include "include/AMCFileParser.h"
#include "include/Skeleton.h"
#include "include/Animation.h"
#include "include/Bone.h"
#include "include/AnimationDataResize.h"
#include "include/AnimationDataMatrixSetup.h"
#include "include/GLM/glm.hpp"
#include "include/GLM/gtx/transform.hpp"
#include "include/GLM/gtc/matrix_transform.hpp"
//#include "include/GLM/gtc/quaternion.hpp"
#include "include/GLM/gtx/quaternion.hpp"
#include "fstream"
#include <algorithm>
#include <iostream>
#include <cctype>

void AMCFileParser::boneDataindexing(std::vector<AnimationData*>& indexVector)
{
    std::ifstream ifs(_filePath);
    std::string buffer;
    uint32 maxBone = _skeleton->getBoneVector().size()-2;
    uint32 moveBone = 0;

    while (std::getline(ifs,buffer))
    {
        if (buffer[0] !='#' && buffer[0] != ':')
            break;
    }
    while (moveBone < maxBone)
    {
        std::getline(ifs,buffer,' ');
        uint32 boneIndex = _skeleton->findBoneIndex(buffer);
        indexVector.push_back(_animation->returnAnimationData(boneIndex));
        std::getline(ifs,buffer);
        moveBone++;
    }
    ifs.close();
}

//캐싱, resize
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

    std::vector<AnimationData*> boneIndexVector;
    boneDataindexing(boneIndexVector);

    AnimationDataResize dataResizer(_total);
    _animation->AnimationDataTraver(dataResizer);

    ifs.open(_filePath);
    while (std::getline(ifs,buffer))
    {
        if (buffer[0] !='#' && buffer[0] != ':')
            break;
    }

    uint32 animationFrame = 0;
    uint32 moveBoneIndex = 0;
    while (ifs >> buffer)
    {
        
        if (moveBoneIndex == boneIndexVector.size())
        {
            moveBoneIndex = 0;
            animationFrame = std::stoi(buffer)-1;
            continue;
        }

        AnimationData* animationData = boneIndexVector[moveBoneIndex];
        std::vector<DOF> dofs = _skeleton->getBoneVector()[animationData->_boneIndex]._dof;

        glm::mat4 matrix(1.0f);
        for (DOF dof : dofs)
        {
            float val;
            ifs >> val;

            if (dof == DOF::RX)
                matrix = glm::rotate(glm::radians(val), glm::vec3(1.0f,0.0f,0.0f)) * matrix;   
            else if (dof == DOF::RY)
                matrix = glm::rotate(glm::radians(val), glm::vec3(0.0f,1.0f,0.0f)) * matrix; 
            else if (dof == DOF::RZ)
                matrix = glm::rotate(glm::radians(val), glm::vec3(0.0f,0.0f,1.0f)) * matrix; 
            else if (dof == DOF::TX)
                animationData->_localTrans[animationFrame].x += val;
            else if (dof == DOF::TY)
                animationData->_localTrans[animationFrame].y += val;
            else if (dof == DOF::TZ)
                animationData->_localTrans[animationFrame].z += val;
        }
        //glm::quat test = unpackQuaternionData(packQuaternionData(glm::quat_cast(matrix)));
        //animationData->_localRotation[animationFrame] = test;
        animationData->_localRotation[animationFrame] = glm::quat_cast(matrix);
        moveBoneIndex++;
    }
    return true;
}