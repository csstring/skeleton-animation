#pragma once
#include "../Common.h"
#include "../BoneLocal.h"
#include "IKInterface.h"

class FootIK : public IKInterface
{
    private:
        void positionFixLimitAngleForWard(glm::vec3& start, glm::vec3& end, glm::vec3 startBoneDir,const Bone& endBone);
        void positionFixLimitAngleBackWard(glm::vec3& start, glm::vec3& end, glm::vec3 endBoneDir,const Bone& endBone);
        bool isOffGroundCheck(const std::vector<glm::vec3>& inCharLocalPos, physx::PxScene* gScene, glm::mat4 charLocalToWorld);
        void findTargetObject(
            const std::vector<glm::vec3>& inCharLocalPos, 
            physx::PxScene* gScene, 
            glm::mat4 charLocalToWorld,
            glm::vec3 tmpMoveDir
        );
        void blendingRatioUpdate(const std::chrono::steady_clock::time_point& curTime);
        void fixBendingAngle(glm::vec3& start, glm::vec3& mid, glm::vec3& end);
        void saveBlendingAnimation(std::vector<glm::vec3>& inCharLocalPos, std::vector<glm::mat4>& inCharLocalRot, glm::vec3 toRootDir);
   
    private:
        bool      _isSaveAnimation = false;
        glm::vec3 _groundNormal;
        bool      _isOffGround;
        float     _groundHight = -10;
        std::vector<glm::vec3> _bonePos;
        std::vector<glm::quat> _boneRot;
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
            physx::PxScene* gScene
        ) override final;
        void setCharGroundHight(float& charGroundHight);
        void setTestOption(glm::vec3 position, glm::vec3 normal, glm::mat4 charLocalToWorld);
};
