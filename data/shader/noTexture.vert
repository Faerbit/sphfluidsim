#version 330

uniform mat4 vpMatrix;
uniform mat3 normalMatrix;
uniform mat4 modelMatrix;
in vec4 vert;
in vec4 normal;
out vec3 fragNormal;
out vec4 fragPos;

void main()
{
    fragPos = modelMatrix * vert;
    fragNormal = normalize(normalMatrix * vec3(normal));
    gl_Position = vpMatrix * modelMatrix * vert;
}
