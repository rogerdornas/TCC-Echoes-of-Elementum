#version 330
layout (location = 0) in vec2 inPosition;
layout (location = 1) in vec2 inTexCoord;

uniform mat4 uWorldTransform;
uniform mat4 uOrthoProj;

out vec2 fragTexCoord;

void main()
{
    gl_Position = uOrthoProj * uWorldTransform * vec4(inPosition, 0.0, 1.0);
    fragTexCoord = inTexCoord;
}