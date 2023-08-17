#pragma once
#include "../Common.h"
#include "../EnumHeader.h"
class Bone;
class Controller;
class BoneLocal;
class Physx;

class IKInterface
{
    protected :
        //test fix me
        // bool                     _targetOn;
        // float                    _blendingRatio;
        bool                     _isFirst;
        glm::vec3                _targetPosition;
        std::vector<glm::vec3>   _bonedirection;
        std::vector<uint8>       _boneIndexVec;
        float                    _velocity;
        uint32                   _retargetTime;
        const std::vector<Bone>& _boneVector;//지워야 할듯?

    public:
        std::chrono::steady_clock::time_point _prevTime;
        std::chrono::steady_clock::time_point _curTime;
        //test fix me
        float                    _blendingRatio;
        bool                     _targetOn;

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
        void saveVelocity(glm::vec3 beforePos, glm::vec3 curPos);
        virtual void solveIK(
            std::vector<BoneLocal>& _boneLocalVector, 
            const glm::mat4& worldRotation, 
            const glm::mat4& worldTranslate,
            const Controller& _controller,
            const std::chrono::steady_clock::time_point& curTime,
            LowerState beforeState,
            Physx* gScene
        ) = 0;
};