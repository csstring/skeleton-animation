#include "BodyInterface.h"
#include "include/TimeNode.h"
#include <deque>
struct AnimationData;
class Animation;
class Skeleton;
enum class TransFormFix
{
    LOWERFRONT,
    LOWERBACK,
    UPPERFRONT
};
class Character : Body
{   
    private:
        std::vector<uint32>    VAO, VBO, VBC;
        std::vector<glm::mat4> _transForm;
        std::vector<glm::mat4> _backTransForm;
        std::vector<glm::mat4> _upperTransForm;

        glm::mat4              _worldTrans;
        glm::mat4              _worldRotation;

        const Skeleton&        _skeleton;

    public:
        std::deque<std::pair<Animation*, TimeNode>> _upperBodyAnimation;//endtime
        std::deque<std::pair<Animation*, TimeNode>> _lowerBodyAnimation;
        std::deque<std::pair<Animation*, TimeNode>> _lowerBodyBackAnimation;
    
    private :
        void updateTransForm(const AnimationData& node, glm::mat4 wolrdTrans, uint32 keyTime, TransFormFix fix);
        void eraseAnimation(const std::chrono::steady_clock::time_point& curTime);
        void boneBufferMaping(void);     
        void animationBlending(const std::chrono::milliseconds& time, const std::vector<glm::mat4>& mixTrans);
    
    public:
        Character(const Skeleton& skeleton) : _skeleton(skeleton){};
        ~Character(){};
        void initialize(void);
        void update(const std::chrono::steady_clock::time_point& curTime) override;
        void draw(void) override;
        void rotationY(float radian);
};
