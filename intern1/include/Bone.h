#pragma once
#include "Common.h"
#include <string>
#include <vector>
#include "GLM/ext.hpp"
#include <tuple>

enum class DOF;

struct Bone
{
    uint32           _boneIndex;
    int32            _parentBoneIndex;
    std::string      _boneName;
    float            _length;
    glm::vec3        _direction;
    std::vector<std::tuple<DOF, float,float>> _limits;
    //fixme : 아래쪽 데이터 필요없어서 정리 해야함
    glm::vec3        _postion;
    glm::vec3        _axis;
    std::vector<DOF> _dof;
    glm::quat        _c;
    glm::quat        _invC;
    glm::vec3        _b;
};

