#pragma once
#include "Common.h"
#include "TimeNode.h"
#include <utility>
#include <deque>
#include "BoneLocal.h"
class Animation;
class Skeleton;
class Character;

enum class KeyInput;

class Controller
{
    private:
        const Animation* findAnimation(const std::string& name, const std::vector<Animation>& _animations);
    
    public:
        Controller() : _player(nullptr){};
        ~Controller(){};

        glm::mat4 getMatrixInCharLocal(
            uint32 boneindex,
            const Skeleton& _skeleton,
            std::vector<BoneLocal>& _boneLocalVector) const;
        
        void pushAnimation(const std::string& name, const std::vector<Animation>& _animations, std::deque<std::pair<const Animation*, TimeNode>>& animationDeque);
        const Character* getPlayer(void) const;
        void setPlayer(Character* player);
        void controllPlayer(KeyInput key, const std::vector<Animation>& _animations);
    
    private:
        Character* _player;
};