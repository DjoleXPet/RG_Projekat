#version 450 core
in vec3 FragPos;
in vec3 Normal;

out vec4 FragColor;

struct Material {
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    float shininess;
};

struct SpotLight {
    vec3 position;
    vec3 direction;
    float cutOff;
    float outerCutOff;

    float constant;
    float linear;
    float quadratic;

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

struct DirLight {
    vec3 direction;
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

uniform SpotLight spotLight;
uniform Material material;
uniform DirLight dirLight;
uniform vec3 viewPos;

vec3 CalcSpotLight(SpotLight light, vec3 normal, vec3 fragPos, vec3 viewDir);
vec3 CalcDirLight(DirLight light, vec3 normal, vec3 viewDir);


void main()
{

    // properties
    vec3 norm = normalize(Normal);
    vec3 viewDir = normalize(viewPos - FragPos);

    // == =====================================================
    // Our lighting is set up in 3 phases: directional, point lights and an optional flashlight
    // For each phase, a calculate function is defined that calculates the corresponding color
    // per lamp. In the main() function we take all the calculated colors and sum them up for
    // this fragment's final color.
    // == =====================================================
    // phase 1: directional lighting
    vec3 result = CalcDirLight(dirLight, norm, viewDir);

    result += CalcSpotLight(spotLight, norm, FragPos, viewDir);

    FragColor = vec4(result, 1.0);

//         float ambientStrength = 0.1;
//         vec3 ambient = ambientStrength * lightColor * material.ambient;
//
//         vec3 norm = normalize(Normal);
//         vec3 lightPos = normalize(light.direction);
//         float diff = max(dot(norm, lightPos), 0.0);
//
//         vec3 diffuse = diff * lightColor;
//
//         float specularStrength = 0.5;
//         vec3 viewDir = normalize(viewPos - FragPos);
//         vec3 reflectDir = reflect(-lightPos, norm);
//         float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32);
//         vec3 specular = specularStrength * spec * lightColor;
//
//         vec3 result = (ambient + diffuse + specular);
//         FragColor = vec4(result, 1.0);
//
//  //   ambient
//    vec3 ambient = dirLight.ambient * material.ambient;
//
//     // diffuse
//     vec3 norm = normalize(Normal);
//     vec3 lightPos = normalize(dirLight.direction);
//     float diff = max(dot(norm, lightPos), 0.0);
//     vec3 diffuse = dirLight.diffuse * (diff * material.diffuse);
//
//     // specular
//     vec3 viewDir = normalize(viewPos - FragPos);
//
//     vec3 halfwayDir = normalize(lightPos + viewDir);
//     float spec = pow(max(dot(norm, halfwayDir), 0.0), material.shininess);
//
// //     vec3 reflectDir = reflect(-lightPos, norm);
// //     float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
//     vec3 specular = dirLight.specular * (spec * material.specular);
//     vec3 result = ambient + diffuse + specular;
//     FragColor = vec4(result, 1.0);
};

vec3 CalcDirLight(DirLight light, vec3 normal, vec3 viewDir)
{
    vec3 lightDir = normalize(-light.direction);
    // diffuse shading
    float diff = max(dot(normal, lightDir), 0.0);
    // specular shading
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
    // combine results
    vec3 ambient = light.ambient * material.ambient;
    vec3 diffuse = light.diffuse * diff * material.diffuse;
    vec3 specular = light.specular * spec * material.specular;
    return (ambient + diffuse + specular);
}

// calculates the color when using a spot light.
vec3 CalcSpotLight(SpotLight light, vec3 normal, vec3 fragPos, vec3 viewDir)
{
    vec3 lightDir = normalize(light.position - fragPos);
    // diffuse shading
    float diff = max(dot(normal, lightDir), 0.0);
    // specular shading
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
    // attenuation
    float distance = length(light.position - fragPos);
    float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));
    // spotlight intensity
    float theta = dot(lightDir, normalize(-light.direction));
    float epsilon = light.cutOff - light.outerCutOff;
    float intensity = clamp((theta - light.outerCutOff) / epsilon, 0.0, 1.0);
    // combine results
    vec3 ambient = light.ambient * material.ambient;
    vec3 diffuse = light.diffuse * diff * material.diffuse;
    vec3 specular = light.specular * spec * material.specular;
    ambient *= attenuation * intensity;
    diffuse *= attenuation * intensity;
    specular *= attenuation * intensity;
    return (ambient + diffuse + specular);
}