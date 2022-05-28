#type vertex
#version 330 core

layout(location = 0) in vec3 a_Position;
layout(location = 1) in vec4 a_color;
layout(location = 2) in vec2 a_texCoord;
layout(location = 3) in float a_texIndex;
layout(location = 4) in float a_tillingFactor;

uniform mat4 u_viewProjMatrix;

out vec4 v_color;
out vec2 v_texCoord;
out float v_texIndex;
out float v_tillingFactor;

void main()
{
	v_tillingFactor = a_tillingFactor;
	v_texIndex = a_texIndex;
	v_color = a_color;
	v_texCoord = a_texCoord;
	gl_Position = u_viewProjMatrix * vec4(a_Position, 1.0); 
}

#type fragment
#version 330 core

layout(location = 0) out vec4 color;

in vec4 v_color;
in vec2 v_texCoord;
in float v_texIndex;
in float v_tillingFactor;

uniform sampler2D u_texture[32];

void main()
{
	vec4 texColor = v_color;
	switch(int(v_texIndex))
	{
		case 0: texColor *= texture(u_texture[int(0)], v_texCoord * v_tillingFactor); break;
		case 1: texColor *= texture(u_texture[int(1)], v_texCoord * v_tillingFactor); break;
		case 2: texColor *= texture(u_texture[int(2)], v_texCoord * v_tillingFactor); break;
		case 3: texColor *= texture(u_texture[int(3)], v_texCoord * v_tillingFactor); break;
		case 4: texColor *= texture(u_texture[int(4)], v_texCoord * v_tillingFactor); break;
		case 5: texColor *= texture(u_texture[int(5)], v_texCoord * v_tillingFactor); break;
		case 6: texColor *= texture(u_texture[int(6)], v_texCoord * v_tillingFactor); break;
		case 7: texColor *= texture(u_texture[int(7)], v_texCoord * v_tillingFactor); break;
		case 8: texColor *= texture(u_texture[int(8)], v_texCoord * v_tillingFactor); break;
		case 9: texColor *= texture(u_texture[int(9)], v_texCoord * v_tillingFactor); break;
		case 10: texColor *= texture(u_texture[int(10)], v_texCoord * v_tillingFactor); break;
		case 11: texColor *= texture(u_texture[int(11)], v_texCoord * v_tillingFactor); break;
		case 12: texColor *= texture(u_texture[int(12)], v_texCoord * v_tillingFactor); break;
		case 13: texColor *= texture(u_texture[int(13)], v_texCoord * v_tillingFactor); break;
		case 14: texColor *= texture(u_texture[int(14)], v_texCoord * v_tillingFactor); break;
		case 15: texColor *= texture(u_texture[int(15)], v_texCoord * v_tillingFactor); break;
		case 16: texColor *= texture(u_texture[int(16)], v_texCoord * v_tillingFactor); break;
		case 17: texColor *= texture(u_texture[int(17)], v_texCoord * v_tillingFactor); break;
		case 18: texColor *= texture(u_texture[int(18)], v_texCoord * v_tillingFactor); break;
		case 19: texColor *= texture(u_texture[int(19)], v_texCoord * v_tillingFactor); break;
		case 21: texColor *= texture(u_texture[int(21)], v_texCoord * v_tillingFactor); break;
		case 22: texColor *= texture(u_texture[int(22)], v_texCoord * v_tillingFactor); break;
		case 23: texColor *= texture(u_texture[int(23)], v_texCoord * v_tillingFactor); break;
		case 24: texColor *= texture(u_texture[int(24)], v_texCoord * v_tillingFactor); break;
		case 25: texColor *= texture(u_texture[int(25)], v_texCoord * v_tillingFactor); break;
		case 26: texColor *= texture(u_texture[int(26)], v_texCoord * v_tillingFactor); break;
		case 27: texColor *= texture(u_texture[int(27)], v_texCoord * v_tillingFactor); break;
		case 28: texColor *= texture(u_texture[int(28)], v_texCoord * v_tillingFactor); break;
		case 29: texColor *= texture(u_texture[int(29)], v_texCoord * v_tillingFactor); break;
		case 30: texColor *= texture(u_texture[int(30)], v_texCoord * v_tillingFactor); break;
		case 31: texColor *= texture(u_texture[int(31)], v_texCoord * v_tillingFactor); break;
	}
    color =  texColor;
}