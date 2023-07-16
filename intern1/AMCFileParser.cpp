#include "include/AMCFileParser.h"
#include "include/Skeleton.h"
#include "include/Animation.h"
#include "include/AnimationDataResize.h"
#include "include/GLM/ext.hpp"
#include "include/GLM/gtx/quaternion.hpp"
#include "include/GLM/gtx/transform.hpp"
#include "include/Quantization.h"
#include "fstream"

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

    uint32 animationTime = 0;
    uint32 moveBoneIndex = 0;
    glm::vec3 firstTrans;
    glm::mat4 firstRot;
    while (ifs >> buffer)
    {
        
        if (moveBoneIndex == boneIndexVector.size())
        {
            ///fix me
            uint32 bone1 = _skeleton->findBoneIndex("lhipjoint");
            uint32 bone2 = _skeleton->findBoneIndex("rhipjoint");
            Bone bone3 = _skeleton->getBoneVector()[bone1];
            Bone bone4 = _skeleton->getBoneVector()[bone2];

            AnimationData* animationData1 = _animation->returnAnimationData(bone1);
            AnimationData* animationData2 = _animation->returnAnimationData(bone2);
            animationData1->_localRotation.push_back({animationTime, glm::mat4(1.0f)});
            animationData2->_localRotation.push_back({animationTime, glm::mat4(1.0f)});
            animationData1->_localTrans.push_back({animationTime, glm::translate(glm::mat4(1.0f), bone3._b)});
            animationData2->_localTrans.push_back({animationTime, glm::translate(glm::mat4(1.0f), bone4._b)});
            
            moveBoneIndex = 0;
            animationTime = std::stoi(buffer)-1;//fix me 120frame, 60 frame
            continue;
        }

        AnimationData* animationData = boneIndexVector[moveBoneIndex];
        Bone bone = _skeleton->getBoneVector()[animationData->_boneIndex];

        glm::mat4 matrix(1.0f);
        glm::vec3 localTransV(0.0f);
        for (DOF dof : bone._dof)
        {
            float val;
            ifs >> val;

            if (dof == DOF::RX)
                matrix = glm::rotate(glm::radians(val), glm::vec3(1.0f,0.0f,0.0f)) * matrix;   
            else if (dof == DOF::RY)
                matrix = glm::rotate(glm::radians(val), glm::vec3(0.0f,1.0f,0.0f)) * matrix; 
            else if (dof == DOF::RZ)
                matrix = glm::rotate(glm::radians(val), glm::vec3(0.0f,0.0f,1.0f)) * matrix; 
            // else if (dof == DOF::TX)
            //     localTransV.x += val;
            // else if (dof == DOF::TY)
            //     localTransV.y += val;
            // else if (dof == DOF::TZ)
            //     localTransV.z += val;
        }
        if (moveBoneIndex == 0 && animationTime ==0)
        {
            firstTrans = localTransV;
        }
        if (moveBoneIndex == 0)
        {
            glm::vec3 tmp = localTransV;
            localTransV -= firstTrans;
            firstTrans = tmp;
            if (_animation->_name == "idle")
                matrix = glm::rotate(glm::radians(90.0f), glm::vec3(0.0f,1.0f,0.0f)) * matrix;
        }
        glm::quat localRot = bone._c * glm::quat_cast(matrix) * bone._invC;
        animationData->_localRotation.push_back({animationTime, localRot });

        glm::vec3 transV = glm::toMat3(localRot) * bone._b + localTransV;
        animationData->_localTrans.push_back({animationTime, glm::translate(glm::mat4(1.0f), transV)});
        //temp
        moveBoneIndex++;
    }
    //fixme
    uint32 bone1 = _skeleton->findBoneIndex("lhipjoint");
    uint32 bone2 = _skeleton->findBoneIndex("rhipjoint");
    Bone bone3 = _skeleton->getBoneVector()[bone1];
    Bone bone4 = _skeleton->getBoneVector()[bone2];

    AnimationData* animationData1 = _animation->returnAnimationData(bone1);
    AnimationData* animationData2 = _animation->returnAnimationData(bone2);
    animationData1->_localRotation.push_back({animationTime, glm::mat4(1.0f)});
    animationData2->_localRotation.push_back({animationTime, glm::mat4(1.0f)});
    animationData1->_localTrans.push_back({animationTime, glm::translate(glm::mat4(1.0f), bone3._b)});
    animationData2->_localTrans.push_back({animationTime, glm::translate(glm::mat4(1.0f), bone4._b)});

    _animation->_animationMillisecond = std::roundf((float)(animationTime * 1000) / (120.0f * _animation->_animationSpeed));
    return true;
}