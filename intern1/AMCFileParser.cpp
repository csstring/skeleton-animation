#include "include/AMCFileParser.h"
#include "include/Skeleton.h"
#include "include/Animation.h"
#include "include/AnimationDataResize.h"
#include "include/GLM/ext.hpp"
#include "include/GLM/gtx/quaternion.hpp"
#include "include/GLM/gtx/transform.hpp"
#include "include/Quantization.h"
#include "include/EnumHeader.h"
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

void AMCFileParser::dumyBoneInitialize(void)
{
    uint32 bone1 = _skeleton->findBoneIndex("lhipjoint");
    uint32 bone2 = _skeleton->findBoneIndex("rhipjoint");
    Bone bone3 = _skeleton->getBoneVector()[bone1];
    Bone bone4 = _skeleton->getBoneVector()[bone2];

    AnimationData* animationData1 = _animation->returnAnimationData(bone1);
    AnimationData* animationData2 = _animation->returnAnimationData(bone2);
    std::vector<uint32> frameList = {0, 1, _total-2, _total-1};
    for (uint32 i =0; i < _total; ++i)
    {
        animationData1->_localRotation.push_back({i, glm::mat4(1.0f)});
        animationData2->_localRotation.push_back({i, glm::mat4(1.0f)});
        animationData1->_localTrans.push_back({i, bone3._b});
        animationData2->_localTrans.push_back({i, bone4._b});
    }
}

void AMCFileParser::saveTotalFrame(void)
{
    std::string buffer;
    std::ifstream ifs(_filePath);

    if (ifs.is_open() == false)
        ft_assert("file open fail");
    while (std::getline(ifs,buffer))
    {
        if ('0' <= buffer[0] && buffer[0] <= '9')
            _total++;
    }
    ifs.close();
}

bool AMCFileParser::loadAMCFile(void)
{
    std::ifstream ifs;
    std::string buffer;
    static float bugtem = 0;
    saveTotalFrame();
    std::vector<AnimationData*> boneIndexVector;
    boneDataindexing(boneIndexVector);

    AnimationDataResize dataResizer(_total);
    _animation->AnimationDataTraver(dataResizer);
    dumyBoneInitialize();

    ifs.open(_filePath);
    while (std::getline(ifs,buffer))
    {
        if (buffer[0] !='#' && buffer[0] != ':')
            break;
    }

    uint32 animationTime = 0;
    uint32 moveBoneIndex = 0;
    glm::vec3 firstTrans, beforeTrans;
    glm::mat4 firstRot;
    glm::vec3 total(0.0f);
    while (ifs >> buffer)
    {
        
        if (moveBoneIndex == boneIndexVector.size())
        {
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
            else if (dof == DOF::TX)
                localTransV.x += val;
            else if (dof == DOF::TY)
                localTransV.y += val;
            else if (dof == DOF::TZ)
                localTransV.z += val;
        }
        if (moveBoneIndex == 0 && animationTime ==0)
        {
            firstTrans = localTransV;
        }
        if (moveBoneIndex == 0)
        {
            glm::vec3 tmp = localTransV;
        
            localTransV -= firstTrans;
            total += localTransV;
            firstTrans = tmp;
            if (_animation->_name == "idle")
                matrix = glm::rotate(glm::radians(90.0f), glm::vec3(0.0f,1.0f,0.0f)) * matrix;
            else if (_animation->_name == "runJump2" || _animation -> _name == "roll")
            {
                matrix = glm::rotate(glm::radians(180.0f), glm::vec3(0.0f,1.0f,0.0f)) * matrix;
                localTransV = glm::rotate(glm::radians(180.0f), glm::vec3(0.0f,1.0f,0.0f)) * glm::vec4(localTransV,1);
            }
            else if (_animation->_name == "punch")
            {
                matrix = glm::rotate(glm::radians(90.0f), glm::vec3(0.0f,1.0f,0.0f)) * matrix;
            }
            else if (_animation->_name == "golf")
            {
                matrix = glm::rotate(glm::radians(-90.0f), glm::vec3(0.0f,1.0f,0.0f)) * matrix;
            }
        }
        glm::quat localRot = bone._c * glm::quat_cast(matrix) * bone._invC;
        animationData->_localRotation.push_back({animationTime, localRot});

        glm::vec3 transV = glm::toMat3(localRot) * bone._b + localTransV;
        animationData->_localTrans.push_back({animationTime, transV});

        moveBoneIndex++;
    }

    _animation->_animationMillisecond = std::roundf((float)(animationTime * 1000) / (120.0f * _animation->_animationSpeed));

    return true;
}