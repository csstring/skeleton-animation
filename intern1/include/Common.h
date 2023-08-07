#pragma once
#include <cassert>
#include <iostream>
#include <string>
#include <vector>
#include <chrono>
#include "physx/PxPhysicsAPI.h"
#include "GLM/glm.hpp"
#include "GLM/gtx/string_cast.hpp"
typedef signed char         int8;
typedef signed short        int16;
typedef signed int          int32;
typedef signed long         int64;
typedef unsigned char       uint8;
typedef unsigned short      uint16;
typedef unsigned int        uint32;
typedef unsigned long       uint64;        

const float PI = 3.141592;
const int WINDOW_WITH = 960;
const int WINDOW_HEIGHT = 540;
const float OVERLAPTIME = 200;

class Noncopyable
{
    public:
        inline ~Noncopyable(void){}
    protected:
        inline Noncopyable(void){}
    private:
        Noncopyable(const Noncopyable&) = delete;
        Noncopyable(Noncopyable&) = delete;
        Noncopyable& operator=(const Noncopyable&) = delete;
        Noncopyable& operator=(Noncopyable&) = delete;
};

//debug
void ft_assert(const std::string& exec);

//utility
std::vector<std::string> ft_split(const std::string& str);
glm::mat4 ft_rotate(glm::vec3 start, glm::vec3 end);
glm::vec3 quatToEulerDivideRatio(const glm::quat& quat, float ratio);
glm::quat quatDivideRatio(const glm::quat& quat, float ratio);

inline std::chrono::steady_clock::time_point getCurTimePoint(void)
{
    return std::chrono::steady_clock::now();
}

inline std::chrono::steady_clock::time_point getAfterTimePoint(float time)
{
    return std::chrono::steady_clock::now() + std::chrono::duration_cast<std::chrono::steady_clock::duration>(std::chrono::duration<double>(time/1000));
}