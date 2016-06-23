#version 330

uniform mat4 mvpMatrix;
uniform mat3 normalMatrix;
uniform mat4 modelMatrix;
in vec4 vert;
in vec4 normal;
in vec4 texCoord;
out vec4 fragTexCoord;
out vec3 fragNormal;
out vec4 fragPos;

void main()
{
    fragPos = modelMatrix * vert;
    fragTexCoord = texCoord;
    fragNormal = normalize(normalMatrix * vec3(normal));
    gl_Position = mvpMatrix * vert;
}
