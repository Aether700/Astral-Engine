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

	v_normal = normalize(vec3(a_transform * vec4(a_normal, 0.0f)));
	v_textureCoords = a_textureCoords;
	v_color = a_color * u_matColor;
}


#type fragment
#version 330 core

layout(location = 0) out vec4 color;

in vec4 v_color;
in vec3 v_position;
in vec3 v_normal;
in vec2 v_textureCoords;

uniform sampler2D u_diffuseMap;
uniform sampler2D u_specularMap;
uniform float u_matShininess;

uniform vec3 u_camPos;
uniform vec3 u_lightPos;
uniform vec3 u_lightAmbient;
uniform vec3 u_lightDiffuse;
uniform vec3 u_lightSpecular;

void main()
{
	vec4 baseColor = texture(u_diffuseMap, v_textureCoords) * v_color;

	// ambient
    vec3 ambient = u_lightAmbient * baseColor.rgb;
  	
    // diffuse 
    vec3 lightDir = normalize(u_lightPos - v_position);
    float diff = max(dot(v_normal, lightDir), 0.0);
    vec3 diffuse = u_lightDiffuse * diff * baseColor.rgb;  
    
    // specular
    vec3 viewDir = normalize(u_camPos - v_position);
    vec3 reflectDir = reflect(-lightDir, v_normal);  
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), u_matShininess);
    vec3 specular = u_lightSpecular * spec * texture(u_specularMap, v_textureCoords).rgb;  
        
    vec3 result = ambient + diffuse + specular;

	//color = baseColor;
	color = vec4(result, baseColor.a);
}