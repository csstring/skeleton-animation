#pragma once
#include "Common.h"
struct BoneLocal
{
    glm::quat rotationInBoneLocal;
    glm::vec3 translationInBoneLocal;
    glm::vec3 scaleInBoneLocal;
};