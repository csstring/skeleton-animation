#include "include/Character.h"
#include "include/Skeleton.h"
#include "include/TimeNode.h"
#include "include/Animation.h"
#include "include/GL/glew.h"
#include "include/Body/Cylinder.h"
#include "include/GLM/gtx/compatibility.hpp"
#include "include/GLM/gtx/quaternion.hpp"
#include "include/Controller.h"
#include "include/IK/EyeIK.h"
#include "include/IK/FootIK.h"
#include <queue>
#include "include/AnimationBlend/Blender.h"
#include "include/AnimationBlend/IBlendNode.h"
#include "include/Body/Ground.h"
#include "include/Body/CollisionCylinder.h"

void Character::setTestLegIK(bool _isRight)
{
    glm::vec3 t, pos;
    if (_isRight == true)
    {
        t = _worldTrans * _worldRotation * _controller.getMatrixInCharLocal(BONEID::RFOOT, _skeleton, _boneLocalVector) * glm::vec4(0,0,0,1);
        pos = {t.x + 3, t.y +4, 1};
    }
    if (_isRight == false)
    {
        t = _worldTrans * _worldRotation * _controller.getMatrixInCharLocal(BONEID::LFOOT, _skeleton, _boneLocalVector) * glm::vec4(0,0,0,1);
        pos = {t.x + 3, t.y +4, -1};
    }

    if (_RfootIK->_targetOn == false && _RfootIK->_blendingRatio == 0)
    {
        _RfootIK->setTestOption(pos, glm::vec3(0,1,0), _worldTrans*_worldRotation);
        std::cout << "test call" << std::endl;
    }
}

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
    _eyeIK->initialize(BONEID::HEAD, BONEID::UPPERBACK);
    _RfootIK = new FootIK(_skeleton.getBoneVector());
    _RfootIK->initialize(BONEID::RFOOT, BONEID::ROOT);

    _LfootIK = new FootIK(_skeleton.getBoneVector());
    _LfootIK->initialize(BONEID::LFOOT, BONEID::ROOT);
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
    glm::vec3 t = _worldTrans * _worldRotation * _controller.getMatrixInCharLocal(BONEID::RFOOT, _skeleton, _boneLocalVector) * glm::vec4(0,0,0,1);
    glm::vec3 root = _worldTrans * _worldRotation * _controller.getMatrixInCharLocal(BONEID::ROOT, _skeleton, _boneLocalVector) * glm::vec4(0,0,0,1);

    // if (t.y > _groundHight)//fix me lastcall
    //     root.y -= _yError * deltaTime;
    // else if (t.y < _groundHight)
    //     root.y += _yError * deltaTime;
    _worldTrans = glm::translate(glm::mat4(1.0f), root);
}

void Character::update(const std::chrono::steady_clock::time_point& curTime, glm::vec3 eyeTarget, Physx* physx)
{
    std::chrono::milliseconds delta;
    static int i = 0;
    if (_isFirst == true)
    {
        _isFirst = false;
        _lastCallTime = curTime;
    }
    if (_yError != 0)
        delta = std::chrono::duration_cast<std::chrono::milliseconds>(curTime - _lastCallTime);
    else 
        delta = std::chrono::duration_cast<std::chrono::milliseconds>(curTime - curTime);

    _blender.eraseAnimationCall(curTime);
    worldPositionUpdate(delta.count());
    _blender.animationUpdate(curTime, _boneLocalVector, _lowerState, _upState);
    // _collisionMesh->update(_worldTrans);

    // _eyeIK->setTargetPosition(eyeTarget);
    // _eyeIK->solveIK(_boneLocalVector, _worldRotation, _worldTrans, _controller, curTime);

    _RfootIK->solveIK(_boneLocalVector, _worldRotation, _worldTrans, _controller, curTime, _lowerState, physx);
    _RfootIK->setCharGroundHight(_groundHight);

    _LfootIK->solveIK(_boneLocalVector, _worldRotation, _worldTrans, _controller, curTime, _lowerState, physx);
    _LfootIK->setCharGroundHight(_groundHight);
    _lastCallTime = curTime;
}

//up -> draw
void Character::draw(void)
{
    const std::vector<Bone>& boneVector = _skeleton.getBoneVector();
    // _collisionMesh->draw();
    glm::vec3 color(0.9412, 0.7922, 0.2549);
    for(const Bone& bone : boneVector)
    {
        glBindVertexArray(VAO[bone._boneIndex]);
        glm::mat4 toParentDir = _worldTrans * _worldRotation * _controller.getMatrixInCharLocal(bone._boneIndex, _skeleton, _boneLocalVector) * ft_rotate(glm::vec3(0.0,0.0,1.0), -bone._direction);// * glm::inverse(test3);
        Cylinder cylinder(0.2, 1.0f *_skeleton.getGBL() * bone._length ,16, toParentDir);
        cylinder.initialize(color, VBC[bone._boneIndex], static_cast<BONEID>(bone._boneIndex));
        cylinder.render(VBO[bone._boneIndex]);
        // Line line(0.7 *_skeleton.getGBL() * bone._length, toParentDir);
        // line.initialize(color, VBC[bone._boneIndex]);
        // line.render(VBO[bone._boneIndex]);
        glBindVertexArray(0);
    }
}




