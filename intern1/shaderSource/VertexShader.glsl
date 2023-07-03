#version 430 core
// Input vertex data, different for all executions of this shader.
layout(location = 0) in vec4 pos;
layout(location = 1) in mat4 model;
//out vec3 outColor;

uniform mat4 projection;
uniform mat4 view;

void main(){

    //gl_Position = projection * view * model * vec4(pos,1.0);
    gl_Position = projection * view * pos;
    gl_PointSize = 10;
    //outColor = color;
}
