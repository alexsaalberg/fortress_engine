#version 330 core
layout(location = 0) in vec3 vPosition;

uniform mat4 P;
uniform mat4 V;

void main()
{
    gl_Position = P * V * vec4(vPosition, 1.0); 
}
