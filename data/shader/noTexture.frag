#version 330

in vec3 fragNormal;
in vec4 fragPos;
out vec4 fragColor;

vec3 lightPosition = vec3(0.0, 10.0, 0.0);
float diffuseFactor = 0.9;
float ambientFactor = 0.8;
float lightIntensity = 10.0;
vec4 color = vec4(0.0, 0.0, 1.0, 1.0);

void main()
{
    vec3 s = normalize(lightPosition - vec3(fragPos));

    float lightFactor =  lightIntensity *
        diffuseFactor * max(dot(s, vec3(fragNormal)), 0.0);
    fragColor = max(lightFactor * color, ambientFactor * color);
}
