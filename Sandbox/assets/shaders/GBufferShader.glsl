#type vertex
#version 330 core

layout(location = 0) in vec3 a_position;
layout(location = 1) in vec3 a_normal;
layout(location = 2) in vec2 a_textureCoords;
layout(location = 3) in mat4 a_transform;
layout(location = 7) in vec4 a_color;
layout(location = 8) in float a_textureIndex;

uniform mat4 u_viewProjMatrix;
uniform vec4 u_matColor;

out vec4 v_color;
out vec3 v_position;
out vec3 v_normal;
out vec2 v_textureCoords;

void main()
{
    v_position = vec3(a_transform * vec4(a_position, 1.0f));
	gl_Position = u_viewProjMatrix * vec4(v_position, 1.0f);

	v_normal = normalize(mat3(transpose(inverse(a_transform))) * a_normal); // can be optimized if computed on cpu
	v_textureCoords = a_textureCoords;
	v_color = a_color * u_matColor;
}


#type fragment
#version 330 core

//layout(location = 0) out vec3 position;
layout(location = 0) out vec4 position;
//layout(location = 1) out vec3 normal;
layout(location = 1) out vec4 normal;
layout(location = 2) out vec4 color;

in vec4 v_color;
in vec3 v_position;
in vec3 v_normal;
in vec2 v_textureCoords;

uniform sampler2D u_diffuseMap;
uniform sampler2D u_specularMap;

void main()
{
	//position = v_position;
	position = vec4(v_position, 1);
	//normal = v_normal;
	normal = vec4(1, 1, 1, 1);
	//color.rgb = (texture(u_diffuseMap, v_textureCoords) * v_color).rgb;
	color.rgb = texture(u_diffuseMap, v_textureCoords).rgb;
	color.a = texture(u_specularMap, v_textureCoords).r; // store specular component in alpha channel
}