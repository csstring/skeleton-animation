#pragma once
#include "../Common.h"
#include "../BoneLocal.h"
#include "IKInterface.h"
#include "../EnumHeader.h"

class Physx;

class FootIK : public IKInterface
{
    private:
        void positionFixLimitAngleForWard(glm::vec3& start, glm::vec3& end, glm::vec3 startBoneDir,const Bone& endBone);
        void positionFixLimitAngleBackWard(glm::vec3& start, glm::vec3& end, glm::vec3 endBoneDir,const Bone& endBone);
        bool isOffGroundCheck(const std::vector<glm::vec3>& inCharLocalPos, Physx* gScene, glm::mat4 charLocalToWorld);
        bool findTargetObject(
            const std::vector<glm::vec3>& inCharLocalPos, 
            Physx* gScene, 
            glm::mat4 charLocalToWorld,
            glm::vec3 tmpMoveDir
        );
        void blendingRatioUpdate();
        void fixBendingAngle(glm::vec3& start, glm::vec3& mid, glm::vec3& end);
        void saveBlendingAnimation(std::vector<glm::vec3>& inCharLocalPos, std::vector<glm::mat4>& inCharLocalRot);
        bool isStartFindTarget(const std::vector<glm::vec3>& inCharLocalPos);
        float getFirstHitHight(const glm::mat4& charLocalToWorld, const glm::vec3& inCharPos, Physx* physx);
        
    private:
        bool      _isSaveAnimation = false;
        float     _rootRatio = 1;
        bool      _isRootAnimationOn = false;
        glm::vec3 _groundNormal;
        bool      _isOffGround;
        float     _groundHight = -10;
        float     _firstHitHight;
        std::vector<glm::vec3> _bonePos;
        std::vector<glm::quat> _boneRot;
        std::vector<glm::vec3> _inCharLocalPrevPos;
        std::vector<glm::mat4> _inCharLocalPrevRot;
        physx::PxRigidActor* _curTouchBody = nullptr;

    public:
        explicit FootIK(const std::vector<Bone>& boneVector) : IKInterface(boneVector)
        {
            _isOffGround = false;
            _bonePos.resize(4);
            _boneRot.resize(4);
        };
        ~FootIK(){};
        bool reachable(const std::vector<glm::vec3>& inCharacterPos, std::vector<float>& distance, glm::vec3 footPosition);
        virtual void solveIK(
            std::vector<BoneLocal>& _boneLocalVector, 
            const glm::mat4& worldRotation, 
            const glm::mat4& worldTranslate,
            const Controller& _controller,
            const std::chrono::steady_clock::time_point& curTime,
            Physx* gScene
        ) override final;
        bool isAnimationBlendingOn(void);
        void setCharGroundHight(float& charGroundHight);
        void setTestOption(glm::vec3 position, glm::vec3 normal, glm::mat4 charLocalToWorld);

    public :
        LowerState _characterState;
};
