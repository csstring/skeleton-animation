#pragma once
#include "common.h"
#include "GLM/glm.hpp"
#include <string>
#include <vector>

enum class DOF{
    RX,
    RY,
    RZ,
    TX,
    TY,
    TZ
};
class Bone
{
private:
    uint32 VAO, VBO, EBO;
public:
    Bone()//data 구조체로 묶고 memzoro?같은걸로 밀어보려야할듯
    {
        _postion.x = 0;
        _postion.y = 0;
        _postion.z = 0;
    };
    ~Bone(){};

    uint32 _boneIndex;
    float _length;
    std::string _boneName;
    glm::vec3 _postion;
    glm::vec3 _orientation;
    glm::vec3 _color;
    glm::vec3 _direction;
    std::vector<DOF> _dof;
    void setupBone(void);
    void draw(void) const;
};

