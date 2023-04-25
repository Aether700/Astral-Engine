#type vertex
#version 330 core

layout(location = 0) in vec3 a_position;
layout(location = 1) in vec3 a_normal;
layout(location = 2) in vec2 a_textureCoords;
layout(location = 3) in mat4 a_transform;
layout(location = 7) in vec4 a_color;
layout(location = 8) in float a_textureIndex;

uniform mat4 u_viewProjMatrix;

void main()
{
	gl_Position = u_viewProjMatrix * a_transform * vec4(a_position, 1.0);
}


#type fragment
#version 330 core

layout(location = 0) out vec4 color;

uniform vec4 u_matColor;

void main()
{
	color = u_matColor; 
}