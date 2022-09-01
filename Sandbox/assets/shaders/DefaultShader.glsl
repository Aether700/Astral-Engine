#type vertex
#version 330 core

layout(location = 0) in vec2 a_position;
layout(location = 1) in vec2 a_textureCoords;

out vec2 v_textureCoords;

void main()
{
    gl_Position = vec4(a_position, 0, 1);
	v_textureCoords = a_textureCoords;
}


#type fragment
#version 330 core

layout(location = 0) out vec4 color;

//in vec4 v_color;
//in vec3 v_position;
//in vec3 v_normal;
in vec2 v_textureCoords;


uniform sampler2D u_positionGBuffer;
uniform sampler2D u_normalGBuffer;
uniform sampler2D u_colorGBuffer;

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
	vec3 baseColor = texture(u_colorGBuffer, v_textureCoords).rgb;
    vec3 normal = texture(u_normalGBuffer, v_textureCoords).rgb;
    vec3 position = texture(u_positionGBuffer, v_textureCoords).rgb;
    float specularIntensity = texture(u_colorGBuffer, v_textureCoords).a;
    
	// ambient
    vec3 ambient = u_lightAmbient * baseColor;
  	
    // diffuse
    vec3 lightDir = normalize(u_lightPos - position);
    float diff = max(dot(normal, lightDir), 0.0);
    vec3 diffuse = u_lightDiffuse * diff * baseColor;  
    
    // specular
    vec3 viewDir = normalize(u_camPos - position);
    vec3 reflectDir = reflect(-lightDir, normal);  
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), u_matShininess);
    vec3 specular = u_lightSpecular * spec * specularIntensity;  
    
    vec3 result = ambient + diffuse + specular;

	//color = vec4(baseColor, 1.0f);
	color = vec4(specularIntensity, 0, 0, 1.0f);
	//color = vec4(result, 1.0f);
}