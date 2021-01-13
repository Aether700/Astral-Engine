#type vertex
#version 330 core

layout(location = 0) in vec3 a_position;

uniform mat4 u_viewProjMatrix;
uniform mat4 u_transform;

void main()
{
	gl_Position = u_viewProjMatrix * u_transform * vec4(a_position, 1.0);
}


#type fragment
#version 330 core

layout(location = 0) out vec4 color;

void main()
{
	color = vec4(1, 0, 0, 1);
}

