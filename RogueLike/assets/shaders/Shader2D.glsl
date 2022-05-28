#type vertex
#version 330 core

layout(location = 0) in vec3 a_position;
layout(location = 1) in vec3 a_texCoord;
layout(location = 2) in vec3 a_normal;
layout(location = 3) in vec4 a_color;
layout(location = 4) in float a_texIndex;
layout(location = 5) in float a_tileFactor;
layout(location = 6) in float a_uses3DTexture;
layout(location = 7) in float a_ignoresCamPos;
layout(location = 8) in float a_ambiantIntensity;
layout(location = 9) in float a_diffuseIntensity;
layout(location = 10) in float a_specularIntensity;
layout(location = 11) in float a_shininess;
layout(location = 12) in float a_ignoresLighting;

uniform mat4 u_viewProjMatrix;

out vec4 v_color;
out vec3 v_texCoord;
out float v_texIndex;
out float v_tileFactor;

void main()
{
	v_color = a_color;
	v_texCoord = a_texCoord;
	v_texIndex = a_texIndex;
	v_tileFactor = a_tileFactor;

	if (a_ignoresCamPos == 1.0f)
	{
		gl_Position = vec4(a_position, 1.0);
	}
	else
	{
		gl_Position = u_viewProjMatrix * vec4(a_position, 1.0);
	}
}


#type fragment
#version 330 core

layout(location = 0) out vec4 color;

in vec4 v_color;
in vec3 v_texCoord;
in float v_texIndex;
in float v_tileFactor;

uniform sampler2D[8] u_texture;

void main()
{
	color = texture(u_texture[int(v_texIndex)], v_texCoord.xy * v_tileFactor) * v_color; 
}