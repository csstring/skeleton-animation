#pragma once
#include "common.h"
#include "Bone.h"
#include "Skeleton.h"
#include <deque>
#include <iostream>
#include "Animation.h"
struct AnimationData;
enum class KeyInput{
    UP,
    BACK,
    REFT,
    RIGHT,
    RUN,
    ATTACK,
    JUMP
};
const float OVERLAPTIME = 400;
class Simulator : Noncopyable
{
    private:
        std::deque<std::pair<Animation*, std::chrono::steady_clock::time_point>> _upperBodyAnimation;//endtime
        std::deque<std::pair<Animation*, std::chrono::steady_clock::time_point>> _lowerBodyAnimation;
        
        std::vector<uint32>    VAO, VBO, VBC;
        uint32                 _keyCount;//지워야함
        glm::mat4              _modelPos;//스켈레톤으로 옮겨야 하나
        std::vector<glm::mat4> _transForm;
        std::vector<glm::mat4> _backTransForm;

    private :
        void updateTransForm(const AnimationData& node, glm::mat4 wolrdTrans, uint32 keyTime);
        void getFrameIterator(uint32* keyArray, uint32 findKeyTime, const std::vector<std::pair<uint32,glm::quat>>& animationFrame);
        Animation* findAnimation(const std::string& name);
        void eraseAnimation(std::chrono::steady_clock::time_point& curTime);
        void pushAnimation(Animation* pushAnimation, std::deque<std::pair<Animation*, std::chrono::steady_clock::time_point>>& animationDeque);

    public:
        Simulator()
        {
            _modelPos = glm::mat4(1.0f);
        }
        ~Simulator(){}
        void initialize(void);

        //getter
        uint32 getTotalKeyCount(void){return _keyCount;};
        
        void changeAnimation(KeyInput key);
        void boneBufferMaping(void);
        void update(void);
        void draw(void);

    public : 
        Skeleton               _skeleton;
        std::vector<Animation> _animations;
};