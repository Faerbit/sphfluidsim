#version 330

in vec4 fragTexCoord;
in vec3 fragNormal;
in vec4 fragPos;
out vec4 fragColor;
uniform sampler2D texture;

vec3 lightPosition = vec3(0.0, 0.0, 0.0);
float diffuseFactor = 0.8;
float ambientFactor = 0.05;
float specularFactor = 0.9;
float shininess = 100.0;
float lightIntensity = 1.0;
vec4 color = vec4(0.5, 1.0, 0.0, 1.0);

void main()
{
    vec3 s = normalize(lightPosition - vec3(fragPos));
    vec3 v = normalize(-vec3(fragPos));
    vec3 r = reflect(-s, fragNormal);

    float lightFactor =  lightIntensity * (
        ambientFactor +
        diffuseFactor * max(dot(s, vec3(fragNormal)), 0.0) +
        specularFactor * pow(max(dot(r, v), 0.0), shininess));
    fragColor = lightFactor * color;
}
