#pragma once
#include "../Common.h"

class Cube
{
    private:
        std::vector<glm::vec4> _buffer;
        uint32 _VAO, _VBO, _VCO;
        
    public:
        std::vector<glm::vec4> _vertex;
        glm::quat _rot;
        glm::mat4 _translate;
        glm::vec3 _position;
        glm::vec3 _dimenstion;
        glm::vec3 _color;

    private:
        std::vector<glm::vec4> CreateCubeVertices(const glm::vec3& dimensions, const glm::vec3& position);    
    
    public:
        Cube(glm::vec3 dimensions, glm::vec3 position, glm::vec3 color = glm::vec3(0,1,0)) : _dimenstion(dimensions), _position(position), _color(color)
        {
            _rot = glm::quat(1.0f, glm::vec3(0.0f));
            _translate = glm::mat4(1.0f);
        };
        ~Cube(){};
        void initialize(void);
        void draw(void);
        void update(void);
};

