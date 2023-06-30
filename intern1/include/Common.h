#pragma once
#include <cassert>
#include <iostream>
#include <string>
#include <vector>

typedef unsigned long long uint64;
typedef long long int64;
typedef unsigned int uint32;
typedef int int32;

const float PI = 3.141592;

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