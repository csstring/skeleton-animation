#pragma once
#include "Common.h"

class Cube
{
    private:
        std::vector<glm::vec4> _buffer;
        uint32 _VAO, _VBO, _VCO;
    
    public:
        std::vector<glm::vec4> _vertex = {
        {-1.0f,-1.0f,-1.0f,1.0f}, // triangle 1 : begin
        {-1.0f,-1.0f, 1.0f,1.0f},
        {-1.0f, 1.0f, 1.0f,1.0f}, // triangle 1 : end
        {1.0f, 1.0f,-1.0f,1.0f}, // triangle 2 : begin
        {-1.0f,-1.0f,-1.0f,1.0f},
        {-1.0f, 1.0f,-1.0f,1.0f}, // triangle 2 : end
        {1.0f,-1.0f, 1.0f,1.0f},
        {-1.0f,-1.0f,-1.0f,1.0f},
        {1.0f,-1.0f,-1.0f,1.0f},
        {1.0f, 1.0f,-1.0f,1.0f},
        {1.0f,-1.0f,-1.0f,1.0f},
        {-1.0f,-1.0f,-1.0f,1.0f},
        {-1.0f,-1.0f,-1.0f,1.0f},
        {-1.0f, 1.0f, 1.0f,1.0f},
        {-1.0f, 1.0f,-1.0f,1.0f},
        {1.0f,-1.0f, 1.0f,1.0f},
        {-1.0f,-1.0f, 1.0f,1.0f},
        {-1.0f,-1.0f,-1.0f,1.0f},
        {-1.0f, 1.0f, 1.0f,1.0f},
        {-1.0f,-1.0f, 1.0f,1.0f},
        {1.0f,-1.0f, 1.0f,1.0f},
        {1.0f, 1.0f, 1.0f,1.0f},
        {1.0f,-1.0f,-1.0f,1.0f},
        {1.0f, 1.0f,-1.0f,1.0f},
        {1.0f,-1.0f,-1.0f,1.0f},
        {1.0f, 1.0f, 1.0f,1.0f},
        {1.0f,-1.0f, 1.0f,1.0f},
        {1.0f, 1.0f, 1.0f,1.0f},
        {1.0f, 1.0f,-1.0f,1.0f},
        {-1.0f, 1.0f,-1.0f,1.0f},
        {1.0f, 1.0f, 1.0f,1.0f},
        {-1.0f, 1.0f,-1.0f,1.0f},
        {-1.0f, 1.0f, 1.0f,1.0f},
        {1.0f, 1.0f, 1.0f,1.0f},
        {-1.0f, 1.0f, 1.0f,1.0f},
        {1.0f,-1.0f, 1.0f,1.0f},
    };
    glm::mat4 _pos;
        
    public:
        Cube()
        {
            _pos = glm::mat4(1.0f);
            _buffer = _vertex;
        };
        ~Cube(){};
        void initialize(void);
        void draw(void);
        void update(void);
        void cubeSizeChange(float size);
};

