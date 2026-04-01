// Request GLSL 3.3
#version 330

layout (location = 0) in vec2 inPosition;

uniform mat4 uWorldTransform;
uniform mat4 uOrthoProj;

// Passamos a posição local (-0.5 a 0.5) para o Fragment Shader
out vec2 fragLocalPos;

void main()
{
    // Transforma a posição para o espaço da tela
    gl_Position = uOrthoProj * uWorldTransform * vec4(inPosition, 0.0, 1.0);

    // Passa a posição original do quad (antes de escalar/mover)
    fragLocalPos = inPosition;
}