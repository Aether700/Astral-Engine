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

uniform DirectionalLight u_directionalLightArr[#NUM_LIGHTS];
uniform int u_numDirectionalLights;

struct PointLight
{
    vec3 position;
    
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    
    float radius;
};

uniform PointLight u_pointLightArr[#NUM_LIGHTS];
uniform int u_numPointLights;

struct SpotLight 
{
    vec3 position;
    vec3 direction;

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;       
    
    float innerAngle;
    float outerAngle;
  
    float radius;
};

uniform SpotLight u_spotLightArr[#NUM_LIGHTS];
uniform int u_numSpotLights;

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

vec3 CalculatePointLightShading(PointLight light, vec3 baseColor, float specularIntensity,
    vec3 position, vec3 normal, vec3 viewDir)
{
    vec3 distVec = light.position - position;
    vec3 lightDir = normalize(distVec);
    
    // ambient
    vec3 ambient = light.ambient * baseColor;

    // diffuse
    float diff = max(dot(normal, lightDir), 0.0);
    vec3 diffuse = light.diffuse * diff * baseColor;
    
    // specular
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32); // try to add shininess per mat here?
    vec3 specular = light.specular * spec * specularIntensity;
    
    // attenuation
    float distance = length(distVec);
    float sqrtDenominator = (distance / light.radius) + 1.0f;
    float attenuation = 1.0f / (sqrtDenominator * sqrtDenominator);
    
    return (ambient + diffuse + specular) * attenuation;
}

vec3 CalculateSpotLightShading(SpotLight light, vec3 baseColor, float specularIntensity,
    vec3 position, vec3 normal, vec3 viewDir)
{
    vec3 distVec = light.position - position;
    vec3 lightDir = normalize(distVec);
    
    // ambient
    vec3 ambient = light.ambient * baseColor;
    
    // diffuse
    float diff = max(dot(normal, lightDir), 0.0);
    vec3 diffuse = light.diffuse * diff * baseColor;
    
    // specular
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32.0f); // try to add shininess per mat here?
    vec3 specular = light.specular * spec * specularIntensity;
    
    // attenuation
    float distance = length(distVec);
    float sqrtDenominator = (distance / light.radius) + 1.0f;
    float attenuation = 1.0f / (sqrtDenominator * sqrtDenominator);
    
    // intensity
    float angle = dot(lightDir, normalize(-light.direction)); 
    float angleDifference = light.innerAngle - light.outerAngle;
    float intensity = clamp((angle - light.outerAngle) / angleDifference, 0.0, 1.0);
    
    return (ambient + diffuse + specular) * attenuation * intensity;
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

    vec3 viewDir = normalize(u_camPos - position);

    vec3 result = vec3(0, 0, 0);
    for (int i = 0; i < u_numDirectionalLights; i++)
    {
        result += CalculateDirectionalLightShading(u_directionalLightArr[i], baseColor, 
            specularIntensity, normal, viewDir);
    }

    for (int i = 0; i < u_numPointLights; i++)
    {
        result += CalculatePointLightShading(u_pointLightArr[i], baseColor, 
            specularIntensity, position, normal, viewDir);
    }

    for (int i = 0; i < u_numSpotLights; i++)
    {
        result += CalculateSpotLightShading(u_spotLightArr[i], baseColor, 
            specularIntensity, position, normal, viewDir);
    }

    color = vec4(result, 1.0f);
}