#type vertex
#version 330 core

layout(location = 0) in vec2 a_position;

void main()
{
    gl_Position = vec4(a_position, 0, 1);
}


#type fragment
#version 330 core

layout(location = 0) out vec4 color1;
layout(location = 1) out vec4 color2;


void main()
{
    color1 = vec4(1, 0, 0, 1);
    color2 = vec4(0, 1, 0, 1);
}