#version 430 core
// Input vertex data, different for all executions of this shader.
layout(location = 0) in vec4 pos;

uniform mat4 projection;
uniform mat4 view;

void main(){

    gl_Position = projection * view * pos;
    //gl_PointSize = 10;
    //outColor = color;
}
