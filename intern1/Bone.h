#pragma once
#include "common.h"
#include "include/GLM/glm.hpp"
#include <string>

class Bone
{
private:
    uint32 VAO, VBO, EBO;
public:
    Bone(){};
    ~Bone(){};

    uint32 _boneIndex;
    float _length;
    std::string _boneName;
    glm::vec3 _postion;
    glm::vec3 _orientation;
    glm::vec3 _color;
    glm::vec3 _direction;

    void setupBone(void);
    void draw(void) const;
};

