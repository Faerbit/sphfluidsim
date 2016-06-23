#version 330

in vec4 fragTexCoord;
in vec3 fragNormal;
out vec4 fragColor;
uniform float time;
uniform sampler2D texture;
uniform sampler2D utilTexture;

void main()
{
    vec2 distortionMapCoord = vec2(fragTexCoord);
    distortionMapCoord.y -= time * 0.2;
    vec4 distortionValue = texture2D(utilTexture, distortionMapCoord);
    vec2 distortionOffset = distortionValue.xy;
    // scale from [0, 1] to [-1, 1]
    distortionOffset -= (0.5, 0.5);
    distortionOffset *= 2;
    // scale to reduce effect
    distortionOffset *= 0.05;

    // ease effect at the top
    distortionOffset *= (1 - fragTexCoord.y);

    vec2 distortedCoord = vec2(fragTexCoord) + distortionOffset;
    vec4 distortedColor = texture2D(texture, distortedCoord);
    vec4 defaultColor = texture2D(texture, vec2(fragTexCoord));
    fragColor = defaultColor * distortedColor;
}
