#type vertex
#version 330 core

layout(location = 0) in vec2 a_position;

void main()
{
	gl_Position = vec4(a_position, 0.0f, 1.0f);
}


#type fragment
#version 330 core

layout(location = 0) out vec4 color;

void main()
{
	color = vec4(1, 0, 0, 1);
}