#pragma once
#include "Common.h"
#include "GL/glew.h"
#include "GLFW/glfw3.h"
#include "GLM/glm.hpp"
#include "GLM/ext.hpp"

class Shader : Noncopyable
{   
    public :
        explicit    Shader(const char* vertexRelativePath,const char* fragmentRelativePath);
        void        initialize(void);
        void        use(void);
        void        setMat4(const std::string &name, glm::mat4 mat4) const;
        void        setUint(const std::string &name, unsigned int index) const;

        uint32      _programId;//fixme
    private:
        std::string _vertexFullPath;
        std::string _fragmentFullPath;
};
