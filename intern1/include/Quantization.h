#pragma once
#include "Common.h"
#include "GLM/ext.hpp"

struct quatPressData
{
    unsigned x : 15;
    unsigned y : 15;
    unsigned z : 15;
    unsigned other : 3;
};

int32 toInt(float val);
float toFloat(int16 val);
quatPressData packQuaternionData(glm::quat quat);
glm::quat unpackQuaternionData(quatPressData packData);