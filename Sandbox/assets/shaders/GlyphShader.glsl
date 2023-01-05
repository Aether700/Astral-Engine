#type vertex
#version 330 core

layout(location = 0) in vec3 a_position;
layout(location = 3) in mat4 a_transform;

uniform mat4 u_viewProjMatrix;


void main()
{
	gl_Position = u_viewProjMatrix * a_transform * vec4(a_position, 1.0);
}


#type fragment
#version 330 core

layout(location = 0) out vec4 color;

void main()
{
	color = vec4(1, 1, 1, 1);
}