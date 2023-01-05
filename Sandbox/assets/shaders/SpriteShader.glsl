#type vertex
#version 330 core

layout(location = 0) in vec3 a_position;
layout(location = 2) in vec2 a_textureCoords;
layout(location = 3) in mat4 a_transform;
layout(location = 7) in vec4 a_color;
layout(location = 8) in float a_textureIndex;

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
	switch(int(v_textureIndex))
	{
		case 0: color = texture(u_textures[0], v_textureCoords) * v_color; break;
		case 1: color = texture(u_textures[1], v_textureCoords) * v_color; break;
		case 2: color = texture(u_textures[2], v_textureCoords) * v_color; break;
		case 3: color = texture(u_textures[3], v_textureCoords) * v_color; break;
		case 4: color = texture(u_textures[4], v_textureCoords) * v_color; break;
		case 5: color = texture(u_textures[5], v_textureCoords) * v_color; break;
		case 6: color = texture(u_textures[6], v_textureCoords) * v_color; break;
		case 7: color = texture(u_textures[7], v_textureCoords) * v_color; break;
		case 8: color = texture(u_textures[8], v_textureCoords) * v_color; break;
		case 9: color = texture(u_textures[9], v_textureCoords) * v_color; break;
		case 10: color = texture(u_textures[10], v_textureCoords) * v_color; break;
		case 11: color = texture(u_textures[11], v_textureCoords) * v_color; break;
		case 12: color = texture(u_textures[12], v_textureCoords) * v_color; break;
		case 13: color = texture(u_textures[13], v_textureCoords) * v_color; break;
		case 14: color = texture(u_textures[14], v_textureCoords) * v_color; break;
		case 15: color = texture(u_textures[15], v_textureCoords) * v_color; break;
		case 16: color = texture(u_textures[16], v_textureCoords) * v_color; break;
		case 17: color = texture(u_textures[17], v_textureCoords) * v_color; break;
		case 18: color = texture(u_textures[18], v_textureCoords) * v_color; break;
		case 19: color = texture(u_textures[19], v_textureCoords) * v_color; break;
		case 20: color = texture(u_textures[20], v_textureCoords) * v_color; break;
		case 21: color = texture(u_textures[21], v_textureCoords) * v_color; break;
		case 22: color = texture(u_textures[22], v_textureCoords) * v_color; break;
		case 23: color = texture(u_textures[23], v_textureCoords) * v_color; break;
		case 24: color = texture(u_textures[24], v_textureCoords) * v_color; break;
		case 25: color = texture(u_textures[25], v_textureCoords) * v_color; break;
		case 26: color = texture(u_textures[26], v_textureCoords) * v_color; break;
		case 27: color = texture(u_textures[27], v_textureCoords) * v_color; break;
		case 28: color = texture(u_textures[28], v_textureCoords) * v_color; break;
		case 29: color = texture(u_textures[29], v_textureCoords) * v_color; break;
		case 30: color = texture(u_textures[30], v_textureCoords) * v_color; break;
		case 31: color = texture(u_textures[31], v_textureCoords) * v_color; break;

		default:
			// display missing texture index color
			color = vec4(1, 0, 1, 1);
			break;
	}
}