#include "include/Character.h"
#include "include/Skeleton.h"
#include "include/TimeNode.h"
#include "include/Animation.h"
#include "include/GL/glew.h"
#include "include/Cylinder.h"
#include "include/GLM/gtx/compatibility.hpp"
#include "include/GLM/gtx/quaternion.hpp"
#include "include/Controller.h"
#include "include/EyeIK.h"
#include <queue>
#include "include/AnimationBlend/Blender.h"
#include "include/AnimationBlend/IBlendNode.h"

void Character::rotationY(float radian)
{
    _worldRotation = glm::rotate(_worldRotation, radian, glm::vec3(0,1,0));
}

glm::mat4 Character::getCharacterWorldPosition(void) const
{
    return _worldTrans * _worldRotation;
}

void Character::initialize(void)
{
    int64 size = _skeleton.getBoneVector().size();

    VAO.resize(size);
    VBO.resize(size);
    VBC.resize(size);

    _boneLocalVector.resize(size);
    _worldTrans = glm::mat4(1.0f);
    _worldRotation = glm::mat4(1.0f);
    boneBufferMaping();

    _blender.initialize();
    _eyeIK = new EyeIK(_skeleton.getBoneVector());
    _eyeIK->initialize();
}

void Character::boneBufferMaping(void)
{
    const std::vector<Bone>& boneVector = _skeleton.getBoneVector();
    uint32 boneIndex = 0;
    for(const Bone& bone : boneVector)
    {
        glGenVertexArrays(1, &VAO[bone._boneIndex]);
        glBindVertexArray(VAO[bone._boneIndex]);

        glGenBuffers(1, &VBO[bone._boneIndex]);
        glBindBuffer(GL_ARRAY_BUFFER, VBO[bone._boneIndex]);
        glEnableVertexAttribArray(0);	
        glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 0, (void*)0);//size 열의 개수
        glBindBuffer(GL_ARRAY_BUFFER, 0);

        glGenBuffers(1, &VBC[bone._boneIndex]);
        glBindBuffer(GL_ARRAY_BUFFER, VBC[bone._boneIndex]);
        glEnableVertexAttribArray(1);	
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);//size 열의 개수
        glBindBuffer(GL_ARRAY_BUFFER, 0);

        glBindVertexArray(0);
    }
}

void Character::worldPositionUpdate(float deltaTime)
{
    glm::vec3 t = _worldTrans * _worldRotation * _controller.getMatrixInCharLocal(0, _skeleton, _boneLocalVector) * glm::vec4(0,0,0,1);
    if (t.y > 0)
        t.y -= _yError * deltaTime;
    // std::cout << glm::to_string(t) << std::endl;
    _worldTrans = glm::translate(glm::mat4(1.0f), t);
}

void Character::stateChange()
{
    // const std::string& name = _blender.getBlendNode(BlendNode::BASE)->_animations.begin()->first->_name;
    //상체가 비어있거나 하체 비어있으면 베이스 애니메이션 state로 채워주기
    // if (name == "run")
    //     _state = PlayerState::RUN;
    // if (name == "idle")
    //     _state = PlayerState::IDLE;
    // if (name == "walk")
    //     _state = PlayerState::WALK;
}

void Character::update(const std::chrono::steady_clock::time_point& curTime, glm::vec3 eyeTarget)
{
    std::chrono::milliseconds delta;
    if (_yError != 0)
        delta = std::chrono::duration_cast<std::chrono::milliseconds>(curTime - _lastCallTime);
    else 
        delta = std::chrono::duration_cast<std::chrono::milliseconds>(curTime - curTime);

    _blender.eraseAnimationCall(curTime);
    stateChange();
    worldPositionUpdate(delta.count());
    _blender.animationUpdate(curTime, _boneLocalVector, _lowerState, _upState);

    _eyeIK->setTargetPosition(eyeTarget);
    _eyeIK->solveEyeIK(_boneLocalVector, _worldRotation, _worldTrans, _controller, curTime);

    _lastCallTime = curTime;
}

//up -> draw
void Character::draw(void)
{
    const std::vector<Bone>& boneVector = _skeleton.getBoneVector();

    glm::vec3 color(0.9412, 0.7922, 0.2549);
    for(const Bone& bone : boneVector)
    {
        glBindVertexArray(VAO[bone._boneIndex]);
        glm::mat4 toParentDir = _worldTrans * _worldRotation * _controller.getMatrixInCharLocal(bone._boneIndex, _skeleton, _boneLocalVector) * ft_rotate(glm::vec3(0.0,0.0,1.0), -bone._direction);// * glm::inverse(test3);
        Cylinder cylinder(0.2, 1.0 *_skeleton.getGBL() * bone._length ,16, toParentDir);
        cylinder.initialize(color, VBC[bone._boneIndex], static_cast<BONEID>(bone._boneIndex));
        cylinder.render(VBO[bone._boneIndex]);
        // Line line(0.7 *_skeleton.getGBL() * bone._length, toParentDir);
        // line.initialize(color, VBC[bone._boneIndex]);
        // line.render(VBO[bone._boneIndex]);
        glBindVertexArray(0);
    }
}




