#pragma once
#include "../Common.h"

class Cube
{
    private:
        std::vector<glm::vec4> _buffer;
        uint32 _VAO, _VBO, _VCO;
        
    public:
        std::vector<glm::vec4> _vertex;
        glm::mat4 _pos;
        glm::vec3 _position;
        glm::vec3 _dimenstion;

    private:
        std::vector<glm::vec4> CreateCubeVertices(const glm::vec3& dimensions, const glm::vec3& position);    
    
    public:
        Cube(glm::vec3 dimensions, glm::vec3 position) : _dimenstion(dimensions), _position(position)
        {
            _pos = glm::mat4(1.0f);
        };
        ~Cube(){};
        void initialize(void);
        void draw(void);
        void update(void);
        void cubeSizeChange(float size);
};

