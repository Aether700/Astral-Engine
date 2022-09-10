#type vertex
#version 330 core

layout(location = 0) in vec3 a_position;
layout(location = 1) in vec2 a_textureCoords;
layout(location = 2) in mat4 a_transform;
layout(location = 6) in vec4 a_color;
layout(location = 7) in float a_textureIndex;

uniform mat4 u_viewProjMatrix;

out vec4 v_color;
out vec2 v_textureCoords;
flat out float v_textureIndex;

void main()
{
	v_textureCoords = a_textureCoords;
	v_textureIndex = a_textureIndex;
	v_color = a_color;
	gl_Position = u_viewProjMatrix * a_transform * vec4(a_position, 1.0);
}


#type fragment
#version 330 core

layout(location = 0) out vec4 color;

in vec4 v_color;
in vec2 v_textureCoords;
flat in float v_textureIndex;

uniform sampler2D u_textures[#NUM_TEXTURE_SLOTS];

void main()
{
	color = texture(u_textures[int(v_textureIndex)], v_textureCoords) * v_color;
}