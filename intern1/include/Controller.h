#pragma once
#include "Common.h"
#include "Body/Cube.h"
#include <deque>
class Animation;
class Skeleton;
class Character;
class TimeNode;
struct BoneLocal;
enum class KeyInput;
enum BlendNode;
class Controller
{
    private:
        const Animation* findAnimation(const std::string& name, const std::vector<Animation>& _animations);
    
    public:
        Controller() : _player(nullptr), _targetCharterCube(glm::vec3(0.3f), glm::vec3(0.0f)){};
        ~Controller(){};
        void initialize(void);
        void draw(void);
        void update(void);

        glm::mat4 getMatrixInCharLocal(uint32 boneindex,const Skeleton& _skeleton, const std::vector<BoneLocal>& _boneLocalVector) const;
        void pushAnimation(const std::string& name, const std::vector<Animation>& _animations, BlendNode nodeNum);
        
        void controllPlayer(KeyInput key, const std::vector<Animation>& _animations);
        Character* getPlayer(void) const {return _player;};
        void setPlayer(Character* player);

    private:
        Cube       _targetCharterCube;
        Character* _player;
};