#pragma once
#include "include/Common.h"

class TimeNode
{
    public:
        explicit TimeNode(
            std::chrono::steady_clock::time_point start,
            std::chrono::steady_clock::time_point end
        ) : _startTime(start) , _endTime(end){}

    public:
        std::chrono::steady_clock::time_point _startTime;
        std::chrono::steady_clock::time_point _endTime;
};