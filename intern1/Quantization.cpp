#include "include/Quantization.h"
#include <cmath>

int16 toShort(float val)
{   
    int16 returnVal = static_cast<int16>(std::roundf(val * (1 << 14)));
    returnVal = returnVal >> 1;
    return returnVal;
}

float toFloat(int16 val)
{
    val = val << 1;
    float returnVal = static_cast<float>(val);
    return returnVal / (1 << 14);
}

quatPressData packQuaternionData(glm::quat quat)
{
    quatPressData pressData;
    uint32 find = 0;
    float max = 0;

    for (uint32 i =0; i <4; ++i)
    {
        float cur = quat[i] * quat[i];
        if (cur > max)
        {
            max = cur;
            find = i;
        }
    }

    pressData.other = find;
    uint32 count = 0;
    for (uint32 i = 0; i < 4; ++i)
    {
        if (i == find)
            continue;
        switch (count++)
        {
        case 0:
            pressData.x = toShort(quat[i]);
            break;
        case 1:
            pressData.y = toShort(quat[i]);
            break;
        case 2:
            pressData.z = toShort(quat[i]);
            break;
        }
    }
    return pressData;
}

glm::quat unpackQuaternionData(quatPressData packData)
{
    glm::quat quat;
    uint32 find = packData.other;

    uint32 count = 0;
    float val = 0;
    for (uint32 i = 0; i < 4; ++i)
    {
        if (i == find)
            continue;
        switch (count++)
        {
        case 0:
            quat[i] = toFloat(packData.x);
            break;
        case 1:
            quat[i] = toFloat(packData.y);
            break;
        case 2:
            quat[i] = toFloat(packData.z);
            break;
        }
        val += quat[i] * quat[i];
    }
    quat[find] = glm::sqrt(1 - val);
    return quat;
}














/*#include "include/Quantization.h"
#include "include/GLM/glm.hpp"
#include <cstring>
#include <cmath>
#include <bitset>
// X = 0, Y = 1, Z, = 2 W, = 3;
const int32 OFFSET = 15;
int32 toInt(float val)
{
    int32 returnVal = 0;
    returnVal = static_cast<int32>(std::round(val* (1 << 14)));

    
    if (returnVal < 0)
    {
        returnVal &= 0x7FFFFFFF;
    }
    returnVal |= (1U << 31);
    return returnVal;
}

float toFloat(uint16 val)
{
    float cutVal = val & 0x7FFF;
    return cutVal / (1 << 15);
}

// X = 0, Y = 1, Z, = 2 W, = 3;

compressedQuat packQuaternionData(glm::quat quat)
{
    uint16 v[3];
    unsigned char* addr = reinterpret_cast<unsigned char*>(v);
    uint32 find = 0;
    uint32 max = 0;


    memset(v,0,sizeof(v));
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
    for (uint32 i = 0; i < 4; ++i)
    {
        if (i == find)
            continue;
        switch (count++)
        {
        case 0:
            reinterpret_cast<uint32&>(v[0]) |= toInt(quat[i]);
            break;
        case 1:
            reinterpret_cast<uint32&>(v[0]) |= (toInt(quat[i]) << 16);
            break;
        case 2:
            reinterpret_cast<uint32&>(v[1]) |= (toInt(quat[i]) << 15);
            break;
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
}*/
