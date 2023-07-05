#include "include/Common.h"
#include "include/GLM/glm.hpp"
#include <cstring>

// X = 0, Y = 1, Z, = 2 W, = 3;
const int32 OFFSET = 15;

compressedQuat packQuaternionData(glm::quat quat)
{
    uint16 v[3];
    unsigned char* addr = reinterpret_cast<unsigned char*>(v);
    uint32 find = 0;
    uint32 max = 0;

    for (uint32 i =0; i <4; ++i)
    {
        uint32 cur = quat[i] * quat[i];
        if (cur > max)
        {
            max = cur;
            find = i;
        }
    }

    uint32 count = 0;
    memcpy(addr + 45, &find,2);
    for (uint32 i = 0; i < 4; ++i)
    {
        if (i != find)
        {
            memcpy(addr + OFFSET*count, &quat[i], 14);
            count++;
        }
    }
    return v;
}

glm::quat unpackQuaternionData(compressedQuat packData)
{
    glm::quat quat;
    unsigned char* addr = reinterpret_cast<unsigned char*>(packData);
    uint32 find;

    memcpy(&find, addr + 45, 2);
    uint32 count = 0;
    float val = 0;
    for (uint32 i = 0; i < 4; ++i)
    {
        if (i != find)
        {
            memcpy(&quat[i], addr + OFFSET*count, 14);
            val += quat[i] * quat[i];
            count++;
        }
    }
    quat[find] = glm::sqrt(1 - val);
    return quat;
}