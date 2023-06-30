#pragma once
#include "Common.h"
#include "include/GL/glew.h"		
#include "include/GLFW/glfw3.h"
#include "include/GLM/ext.hpp"

class Shader : Noncopyable
{
    uint32 _programId;
    public :
        void use(void);
        void LoadShaders(const char* vertex_file_path, const char* fragment_file_path);
    //temp
    /*
    void setInt(const std::string &name, int value) const
    { 
        glUniform1i(glGetUniformLocation(_programId, name.c_str()), value); 
    }
    */
    void setMat4(const std::string &name, glm::mat4 mat4) const
    {
        glUniformMatrix4fv(glGetUniformLocation(_programId, name.c_str()),1, false, glm::value_ptr(mat4));
    }
};