#version 450 core
out vec4 FragColor;

in vec2 TexCoords;

uniform sampler2D scene;
uniform sampler2D bloomBlur;
uniform bool bloom;
uniform float exposure;

void main()
{
    const float gamma = 2.2f;
    vec3 hdrColor = vec3(texture(scene, TexCoords));
    vec3 bloomColor = vec3(256,1,1); // vec3(texture(bloomBlur, TexCoords));
    if(bloom)
       hdrColor += bloomColor; // additive blending
    // tone mapping
    vec3 result = vec3(1.0) - exp(-hdrColor * exposure);

    // also gamma correct while we're at it
    result = pow(result, vec3(1.0 / gamma));
    FragColor = vec4(result, 1.0f);
}