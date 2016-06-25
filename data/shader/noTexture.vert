#version 330

uniform mat4 mvpMatrix;
uniform mat3 normalMatrix;
uniform mat4 modelMatrix;
in vec4 vert;
in vec4 normal;
in vec3 position;
out vec3 fragNormal;
out vec4 fragPos;

void main()
{
    fragPos = modelMatrix * (vert + vec4(position, 0.0));
    fragNormal = normalize(normalMatrix * vec3(normal));
    gl_Position = mvpMatrix * (vert + vec4(position, 0.0));
}
