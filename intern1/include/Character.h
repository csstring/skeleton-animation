#include "BodyInterface.h"
#include "TimeNode.h"
#include <deque>
#include "EyeIK.h"
struct AnimationData;
class Animation;
class Skeleton;
class Controller;

struct BoneLocal
{
    glm::quat rotationInBoneLocal;
    glm::vec3 translationInBoneLocal;
    glm::vec3 scaleInBoneLocal;
};

class Character
{   
    private:
        bool                   _isFirstBlend;
        std::vector<uint32>    VAO, VBO, VBC;
        std::vector<BoneLocal> _boneLocalVector;
        glm::mat4              _worldTrans;
        glm::mat4              _worldRotation;
        const Skeleton&        _skeleton;
        const Controller&      _controller;


    public:
        std::deque<std::pair<const Animation*, TimeNode>> _upperBodyAnimation;//endtime
        std::deque<std::pair<const Animation*, TimeNode>> _baseAnimation;
        std::deque<std::pair<const Animation*, TimeNode>> _lowerBodyAnimation;
        EyeIK* _eyeIK;

    private :
        void updateTransForm(const AnimationData& node, uint32 keyTime, float interpolVal);
        void eraseAnimation(const std::chrono::steady_clock::time_point& curTime);
        void boneBufferMaping(void);     
        void animationBlending(const std::chrono::milliseconds& time, const std::vector<glm::mat4>& mixTrans);
        void getTransFormByKeyFrame(glm::quat& interpolR, glm::vec3& interpolT, const AnimationData* node, uint32 keyFrame);
        glm::mat4 getMatrixInCharLocal(uint32 boneindex);
        void worldPositionUpdate(void);
    public:
        Character(const Skeleton& skeleton, const Controller& controller) : _skeleton(skeleton), _eyeIK(nullptr), _controller(controller){};
        ~Character()
        {
            if (_eyeIK != nullptr)
                delete _eyeIK;
        };
        void initialize(void);
        void update(const std::chrono::steady_clock::time_point& curTime, glm::vec3 eyeTarget);
        void draw(void);
        void rotationY(float radian);
};
