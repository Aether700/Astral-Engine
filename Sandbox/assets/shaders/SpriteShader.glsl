#type vertex
#version 330 core

layout(location = 0) in vec3 a_position;
layout(location = 1) in vec2 a_textureCoords;
layout(location = 2) in mat4 a_transform;
layout(location = 6) in vec4 a_color;

uniform mat4 u_viewProjMatrix;

out vec4 v_color;
out vec2 v_textureCoords;

void main()
{
	v_textureCoords = a_textureCoords;
	v_color = a_color;
	gl_Position = u_viewProjMatrix * a_transform * vec4(a_position, 1.0);
}


#type fragment
#version 330 core

layout(location = 0) out vec4 color;

in vec4 v_color;
in vec2 v_textureCoords;

uniform vec4 u_matColor;
uniform sampler2D u_textures[#NUM_TEXTURE_SLOTS];

void main()
{
	color = texture(u_textures[0], v_textureCoords) * v_color * u_matColor; 
	//color = v_color * u_matColor; 
}