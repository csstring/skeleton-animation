#version 430 core
// Input vertex data, different for all executions of this shader.
layout(location = 0) in vec4 pos;
layout(location = 1) in vec3 Incolor;

uniform mat4 projection;
uniform mat4 view;

out vec4 fragPosition;
out vec3 passColor;
void main(){

    gl_Position = projection * view * pos;
    fragPosition = projection * view * pos;
    passColor = Incolor;
    //gl_PointSize = 10;
    //outColor = color;
}
