#pragma once
#include "Common.h"

class Ground
{
    private:
        const float _groundVertex[16] = {100.0f,-10.0f,100.0f,1.0f, 100.0f,-10.0f,-100.0f,1.0f,-100.0f,-10.0f,100.0f,1.0f,-100.0f,-10.0f,-100.0f,1.0f};
        uint32 _VAO, _VBO, _VCO;
    public:
        Ground(){};
        ~Ground(){};
        void initialize(void);
        void draw(void);
};

