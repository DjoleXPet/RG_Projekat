#version 450 core
in vec3 FragPos;
in vec2 TexCoords;

out vec4 FragColor;

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

uniform sampler2D texture_diffuse1;
// uniform sampler2D texture_normal1;

uniform SpotLight spotLight;
uniform DirLight dirLight;
uniform vec3 viewPos;

vec3 CalcSpotLight(SpotLight light, vec3 normal, vec3 fragPos, vec3 viewDir);
vec3 CalcDirLight(DirLight light, vec3 normal, vec3 viewDir);

void main()
{

    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 norm = vec3(0,1,0);//texture(texture_normal1, TexCoords).rgb;  // ne treba nam normal a ni difuzno svetlo zato sto je podloga ravna

    vec3 result = CalcDirLight(dirLight, norm, viewDir);

    result += CalcSpotLight(spotLight, norm, FragPos, viewDir);

    FragColor = vec4(result, 1.0);
}


// void main(){
//
// //
// //        float ambientStrength = 0.5;
// //        vec3 ambient = ambientStrength * light.ambient;
// //
// //         vec3 norm = normalize(Normal);
// //         vec3 lightPos = normalize(light.direction);
// //         float diff = max(dot(norm, lightPos), 0.0);
// //         vec3 diffuse = diff * light.diffuse;
// //
// //        float specularStrength = 0.5;
// //        vec3 viewDir = normalize(viewPos - FragPos);
// //
// //        vec3 halfwayDir = normalize(lightPos + viewDir);
// //        float spec = pow(max(dot(norm, halfwayDir), 0.0), 4);
// //
// // //         vec3 reflectDir = reflect(-lightPos, norm);
// // //         float spec = pow(max(dot(viewDir, reflectDir), 0.0), 4);
// //        vec3 specular = specularStrength * spec * light.specular;
// //
// //        vec3 result = (ambient + specular + diffuse) * vec3(0.0f, 0.7f, 0.2f);
// //        FragColor = vec4(result, 1.0);
// //
//
//
//
//             // obtain normal from normal map in range [0,1]
//            vec3 norm = texture(texture_normal1, TexCoords).rgb;
//            // transform normal vector to range [-1,1]
//            norm = normalize(norm * 2.0 - 1.0);  // this normal is in tangent space
//
//            // get diffuse color
//            vec3 color = texture(texture_diffuse1, TexCoords).rgb;
//            // ambient
//            vec3 ambient = dirLight.ambient * color;
//            // diffuse
//            vec3 lightPos = normalize(-dirLight.direction);
//            float diff = max(dot(lightPos, norm), 0.0);
//            vec3 diffuse = diff * color * dirLight.diffuse;
//            // specular
//
//            //         vec3 reflectDir = reflect(-lightPos, norm);
//            //         float spec = pow(max(dot(viewDir, reflectDir), 0.0), 4);
//
//            vec3 viewDir = normalize(viewPos - FragPos);
//            vec3 reflectDir = reflect(-lightPos, norm);
//            vec3 halfwayDir = normalize(lightPos + viewDir);
//            float spec = pow(max(dot(norm, halfwayDir), 0.0), 4.0);
//
//            vec3 specular = vec3(0.2) * spec;
//            FragColor = vec4(ambient + diffuse + specular, 1.0);
// };


// calculates the color when using a directional light.
vec3 CalcDirLight(DirLight light, vec3 normal, vec3 viewDir)
{
    vec3 lightDir = normalize(-light.direction);

    // diffuse shading
    float diff = max(dot(normal, lightDir), 0.0);
    // specular shading
 //   vec3 reflectDir = reflect(-lightDir, normal);

    // blinnPhong
    vec3 halfwayDir = normalize(lightDir + viewDir);

    float spec = pow(max(dot(viewDir, halfwayDir), 0.0), 4);
    // combine results
    vec3 ambient = light.ambient * vec3(texture(texture_diffuse1, TexCoords));
    vec3 diffuse = light.diffuse * diff * vec3(texture(texture_diffuse1, TexCoords));
    vec3 specular = light.specular * spec * vec3(texture(texture_diffuse1, TexCoords));
    return (ambient + diffuse + specular);
}

// calculates the color when using a spot light.
vec3 CalcSpotLight(SpotLight light, vec3 normal, vec3 fragPos, vec3 viewDir)
{
    vec3 lightDir = normalize(light.position - fragPos);
    // diffuse shading
    float diff = max(dot(normal, lightDir), 0.0);
    // specular shading
//   vec3 reflectDir = reflect(-lightDir, normal);
    vec3 halfwayDir = normalize(lightDir + viewDir);
    float spec = pow(max(dot(viewDir, halfwayDir), 0.0), 204);
    // attenuation
    float distance = length(light.position - fragPos);
    float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));
    // spotlight intensity
    float theta = dot(lightDir, normalize(-light.direction));
    float epsilon = light.cutOff - light.outerCutOff;
    float intensity = clamp((theta - light.outerCutOff) / epsilon, 0.0, 1.0);
    // combine results
    vec3 ambient = light.ambient * vec3(texture(texture_diffuse1, TexCoords));
    vec3 diffuse = light.diffuse * diff * vec3(texture(texture_diffuse1, TexCoords));
    vec3 specular = light.specular * spec * vec3(texture(texture_diffuse1, TexCoords));
    ambient *= attenuation * intensity;
    diffuse *= attenuation * intensity;
    specular *= attenuation * intensity;
    return (ambient + diffuse + specular*100.0f);
}