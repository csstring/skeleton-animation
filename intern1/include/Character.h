#include "TimeNode.h"
#include <deque>
#include "BoneLocal.h"
#include "AnimationBlend/Blender.h"
#include "EnumHeader.h"
struct AnimationData;
class Animation;
class Skeleton;
class Controller;
class EyeIK;
class FootIK;
class Ground;
class CollisionCylinder;
class Character
{   
    private:
        std::vector<uint32>    VAO, VBO, VBC;
        std::vector<BoneLocal> _boneLocalVector;
        glm::mat4              _worldTrans;
        glm::mat4              _worldRotation;
        const Skeleton&        _skeleton;
        const Controller&      _controller;
        bool                   _isFirst;
        std::chrono::steady_clock::time_point _lastCallTime;

    public:
        CollisionCylinder* _collisionMesh;
        Blender _blender;
        EyeIK* _eyeIK;
        FootIK* _footIK;
        UpperState _upState;
        LowerState _lowerState;
        float      _yError = 0.001;

    private :
        void boneBufferMaping(void);  
        void worldPositionUpdate(float deltaTime);

    public:
        Character(
            const Skeleton& skeleton, const Controller& controller, CollisionCylinder* collisionMesh
        ) : _skeleton(skeleton), _eyeIK(nullptr), _controller(controller), _isFirst(true), _collisionMesh(collisionMesh){};
        ~Character()
        {
            if (_eyeIK != nullptr)
                delete _eyeIK;
        };
        void initialize(void);
        const Skeleton& getCharacterSkeleton(void) const {return _skeleton;};
        void update(const std::chrono::steady_clock::time_point& curTime, glm::vec3 eyeTarget, const Ground& ground);
        void draw(void);
        void rotationY(float radian);
        glm::mat4 getCharacterWorldPosition(void) const;
        const std::vector<BoneLocal>& getCharLocalVector(void) const {return _boneLocalVector;};
};
