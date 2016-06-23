#version 330

in vec3 fragNormal;
out vec4 fragColor;

void main()
{
    fragColor = vec4(fragNormal.x, fragNormal.y, fragNormal.z, 1.0);
}
