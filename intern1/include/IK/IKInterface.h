#pragma once
#include "../Common.h"
#include "../EnumHeader.h"
class Bone;
class Controller;
class BoneLocal;

class IKInterface
{
    protected :
        bool                     _targetOn;
        bool                     _isFirst;
        float                    _blendingRatio;
        glm::vec3                _targetPosition;
        std::vector<glm::vec3>   _bonedirection;
        std::vector<uint8>       _boneIndexVec;
        const std::vector<Bone>& _boneVector;//지워야 할듯?

    public:
        std::chrono::steady_clock::time_point _callTime;
    
    public:
        explicit IKInterface(const std::vector<Bone>& boneVector) : _boneVector(boneVector)
        {
            _targetOn = false;
            _isFirst = true;
            _blendingRatio = 0;
        };
        virtual ~IKInterface(){};

        void initialize(BONEID targetBone, BONEID startBone);
        void setTargetPosition(glm::vec3 targetPosition);
        virtual void solveIK(
            std::vector<BoneLocal>& _boneLocalVector, 
            const glm::mat4& worldRotation, 
            const glm::mat4& worldTranslate,
            const Controller& _controller,
            const std::chrono::steady_clock::time_point& curTime
        ) = 0;
};