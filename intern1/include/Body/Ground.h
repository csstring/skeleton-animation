#pragma once
#include "../Common.h"

class Ground
{
    private:
        std::vector<glm::vec4> _groundVertex;
        // const float _groundVertex[16] = {100.0f,-10.0f,100.0f,1.0f, 100.0f,-10.0f,-100.0f,1.0f,-100.0f,-10.0f,100.0f,1.0f,-100.0f,-10.0f,-100.0f,1.0f};
        uint32 _VAO, _VBO, _VCO;
    public:
        Ground() : _rot(glm::mat4(1.0f)){};
        ~Ground(){};
        void initialize(void);
        void draw(void);
        void update(void);
        glm::vec3 getCenter(void) const;
        glm::mat4 _rot;
        glm::vec3 _normal;
};

