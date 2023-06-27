#pragma once

typedef unsigned long long uint64;
typedef long long int64;
typedef unsigned int uint32;
typedef int int32;

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