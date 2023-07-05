#pragma once
#include <cassert>
#include <iostream>
#include <string>
#include <vector>
#include "include/GLM/gtx/string_cast.hpp"
typedef signed char         int8;
typedef signed short        int16;
typedef signed int          int32;
typedef signed long         int64;
typedef unsigned char       uint8;
typedef unsigned short      uint16;
typedef unsigned int        uint32;
typedef unsigned long       uint64;
typedef uint16*             compressedQuat;          

const float PI = 3.141592;
const int WINDOW_WITH = 1024;
const int WINDOW_HEIGHT = 728;

class Noncopyable
{
public:
    inline ~Noncopyable(void) {}
protected:
    inline Noncopyable(void) {}
private:
    Noncopyable(const Noncopyable&) = delete;
    Noncopyable(Noncopyable&) = delete;
    Noncopyable& operator=(const Noncopyable&) = delete;
    Noncopyable& operator=(Noncopyable&) = delete;
};

//debug
void ft_assert(const std::string& exec);
std::vector<std::string> ft_split(const std::string& str);

//quat
compressedQuat packQuaternionData(glm::quat quat);
glm::quat unpackQuaternionData(compressedQuat packData);