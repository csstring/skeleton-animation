#pragma once
#include "common.h"
#include "Bone.h"
#include "Skeleton.h"
#include <deque>
#include <iostream>
#include "Animation.h"
struct AnimationData;
const float OVERLAPTIME = 200;
enum class KeyInput
{
    UP,
    LOWERBACK,
    REFT,
    RIGHT,
    RUN,
    ATTACK,
    JUMP,
    STOP
};
enum class TransFormFix
{
    LOWERFRONT,
    LOWERBACK,
    UPPERFRONT
};
struct TimeNode
{
    public:
        explicit TimeNode(
            std::chrono::steady_clock::time_point start,
            std::chrono::steady_clock::time_point end
        ) : _startTime(start) , _endTime(end){}

    public:
        std::chrono::steady_clock::time_point _startTime;
        std::chrono::steady_clock::time_point _endTime;
};

class Simulator : Noncopyable
{
    private:
        std::deque<std::pair<Animation*, TimeNode>> _upperBodyAnimation;//endtime
        std::deque<std::pair<Animation*, TimeNode>> _lowerBodyAnimation;
        std::deque<std::pair<Animation*, TimeNode>> _lowerBodyBackAnimation;
        
        std::vector<uint32>    VAO, VBO, VBC;
        std::vector<glm::mat4> _transForm;
        std::vector<glm::mat4> _backTransForm;
        std::vector<glm::mat4> _upperTransForm;
        
        //스켈레톤으로 옮겨야 하나
        glm::mat4              _worldTrans;
        glm::mat4              _worldRotation, _worldRotBuffer;

    private :
        void updateTransForm(const AnimationData& node, glm::mat4 wolrdTrans, uint32 keyTime, TransFormFix fix);
        void getFrameIterator(uint32* keyArray, uint32& findKeyTime, const std::vector<std::pair<uint32,glm::quat>>& animationFrame);
        Animation* findAnimation(const std::string& name);
        void eraseAnimation(std::chrono::steady_clock::time_point& curTime);
        void pushAnimation(Animation* pushAnimation, std::deque<std::pair<Animation*, TimeNode>>& animationDeque);
        void boneBufferMaping(void);     
        void animationBlending(const std::chrono::milliseconds& time);

    public:
        Simulator(){}
        ~Simulator(){}
        void initialize(void);
        void changeAnimation(KeyInput key);
        void update(void);
        void draw(void);
    public : 
        Skeleton               _skeleton;
        std::vector<Animation> _animations;
};

std::ostream& operator<<(std::ostream& os, const std::pair<Animation*, TimeNode>& ref);