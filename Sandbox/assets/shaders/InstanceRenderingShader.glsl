#type vertex
#version 330 core

layout(location = 0) in vec3 a_position;
layout(location = 1) in vec3 a_offset;

void main()
{
	gl_Position = vec4(a_position + a_offset, 1.0);
	//gl_Position = u_viewProjMatrix * vec4(a_position, 1.0);
}


#type fragment
#version 330 core

layout(location = 0) out vec4 color;

void main()
{
	color = vec4(1, 0, 0, 1); 
}