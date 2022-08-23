#type vertex
#version 330 core

layout(location = 0) in vec3 a_position;
layout(location = 1) in mat4 a_transform;
layout(location = 5) in vec4 a_color;

uniform mat4 u_viewProjMatrix;

out vec4 v_color;

void main()
{
	v_color = a_color;
	gl_Position = u_viewProjMatrix * a_transform * vec4(a_position, 1.0);
}


#type fragment
#version 330 core

layout(location = 0) out vec4 color;

in vec4 v_color;

uniform vec4 u_matColor;

void main()
{
	color = v_color * u_matColor; 
}