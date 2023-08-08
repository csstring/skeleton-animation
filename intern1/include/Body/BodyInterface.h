#pragma once
#include "../Common.h"

class Body
{
    public:
        Body(){};
        virtual ~Body(){};

        virtual void update(const std::chrono::steady_clock::time_point&) = 0;
        virtual void draw(void) = 0;
};