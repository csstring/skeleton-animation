#pragma once
#include "include/Common.h"
#include "include/Cube.h"
#include <deque>
class Animation;
class Skeleton;
class Character;
class TimeNode;
struct BoneLocal;
enum class KeyInput;

class Controller
{
    private:
        const Animation* findAnimation(const std::string& name, const std::vector<Animation>& _animations);
    
    public:
        Controller() : _player(nullptr){};
        ~Controller(){};
        void initialize(void);
        void draw(void);
        void update(void);

        glm::mat4 getMatrixInCharLocal(uint32 boneindex,const Skeleton& _skeleton, const std::vector<BoneLocal>& _boneLocalVector) const;
        void pushAnimation(const std::string& name, const std::vector<Animation>& _animations, std::deque<std::pair<const Animation*, TimeNode>>& animationDeque);
        
        void controllPlayer(KeyInput key, const std::vector<Animation>& _animations);
        Character* getPlayer(void) const {return _player;};
        void setPlayer(Character* player);

    private:
        Cube       _targetCharterCube;
        Character* _player;
};