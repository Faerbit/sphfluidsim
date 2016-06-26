#version 330

uniform mat4 mvpMatrix;
uniform mat3 normalMatrix;
uniform mat4 modelMatrix;
uniform float scale;
in vec4 vert;
in vec4 normal;
in vec4 position;
out vec3 fragNormal;
out vec4 fragPos;

void main()
{
    mat4 instMat;
    instMat[0] = vec4(scale, 0.0, 0.0, 0.0);
    instMat[1] = vec4(0.0, scale, 0.0, 0.0);
    instMat[2] = vec4(0.0, 0.0, scale, 0.0);
    instMat[3] = vec4(position.x, position.y, position.z, 1.0);
    fragPos = modelMatrix * instMat * vert;
    fragNormal = normalize(normalMatrix * vec3(normal));
    gl_Position = mvpMatrix * instMat * vert;
}
