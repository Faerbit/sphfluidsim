#version 330

in vec3 fragNormal;
in vec4 fragPos;
out vec4 fragColor;

uniform vec3 cameraPos;

vec3 lightVector = normalize(vec3( -0.25, 1.0, -0.5));
float specularFactor = 0.2;
float diffuseFactor = 0.8;
float ambientFactor = 0.2;
float shininess = 200.0;
float lightIntensity = 7.5;
vec4 color = vec4(0.0, 0.0, 1.0, 1.0);

void main()
{
    float diffuseIntensity = max(dot(fragNormal, lightVector), 0.0);

    vec3 cameraVector = normalize(cameraPos - vec3(fragPos));
    float specularIntensity = pow(
            dot((cameraVector + lightVector), fragNormal)/
            length(cameraVector + lightVector),
            shininess);

    vec4 dsColor =  lightIntensity * (
        clamp(diffuseIntensity, 0.0, 1.0) * diffuseFactor * color +
        clamp(specularIntensity, 0.0, 1.0)) * specularFactor
            * vec4(1.0, 1.0, 1.0, 1.0) ;
    fragColor = max(dsColor, ambientFactor * color);
}
