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

in vec2 v_textureCoords;

uniform sampler2D u_positionGBuffer;
uniform sampler2D u_normalGBuffer;
uniform sampler2D u_colorGBuffer;

uniform sampler2D u_diffuseMap;
uniform sampler2D u_specularMap;

uniform vec3 u_camPos;

struct DirectionalLight
{
    vec3 direction;
  
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

uniform DirectionalLight u_directionalLightArr[50];
uniform int u_numDirectionalLights;

vec3 CalculateDirectionalLightShading(DirectionalLight light, vec3 baseColor, 
    float specularIntensity, vec3 normal, vec3 viewDir)
{
    vec3 lightDir = normalize(-light.direction);
    
    // ambient
    vec3 ambient = light.ambient * baseColor;
    
    // diffuse shading
    float diff = max(dot(normal, lightDir), 0.0);
    vec3 diffuse = light.diffuse * diff * baseColor;
    
    // specular shading
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32); // try to add shininess per mat here?
    vec3 specular = light.specular * spec * specularIntensity;
    
    return ambient + diffuse + specular;
}

void main()
{
    vec3 normal = texture(u_normalGBuffer, v_textureCoords).rgb;
    if (normal == vec3(0, 0, 0))
    {
        discard;
    }

	vec3 baseColor = texture(u_colorGBuffer, v_textureCoords).rgb;
    vec3 position = texture(u_positionGBuffer, v_textureCoords).rgb;
    float specularIntensity = texture(u_colorGBuffer, v_textureCoords).a;
    
    /*
	// ambient
    vec3 ambient = u_lightAmbient * baseColor;
  	
    // diffuse
    vec3 lightDir = normalize(u_lightPos - position);
    float diff = max(dot(normal, lightDir), 0.0);
    vec3 diffuse = u_lightDiffuse * diff * baseColor;  
    
    // specular
    vec3 viewDir = normalize(u_camPos - position);
    vec3 reflectDir = reflect(-lightDir, normal);  
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32.0f);
    vec3 specular = u_lightSpecular * spec * specularIntensity;  
    
    vec3 result = ambient + diffuse + specular;

	color = vec4(result, baseColor.a);
    */

    vec3 viewDir = normalize(u_camPos - position);

    vec3 result = vec3(0, 0, 0);
    for (int i = 0; i < u_numDirectionalLights; i++)
    {
        result += CalculateDirectionalLightShading(u_directionalLightArr[i], baseColor, 
            specularIntensity, normal, viewDir);
    }

    color = vec4(result, 1.0f);
}