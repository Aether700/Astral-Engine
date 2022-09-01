#type vertex
#version 330 core

layout(location = 0) in vec2 a_position;
layout(location = 1) in vec2 a_textureCoords;

out vec2 v_textureCoords;

void main()
{
	gl_Position = vec4(a_position, 0.0f, 1.0f);
	v_textureCoords = a_textureCoords;
}


#type fragment
#version 330 core

layout(location = 0) out vec4 color;

in vec2 v_textureCoords;

uniform sampler2D u_texture;

void main()
{
	//color = texture(u_texture, v_textureCoords);
	color = vec4(texture(u_texture, v_textureCoords).rgb, 1);
}