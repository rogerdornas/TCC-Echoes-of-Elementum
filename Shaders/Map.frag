// Request GLSL 3.3
#version 330

out vec4 outColor;

uniform sampler2D uMapTexture;
uniform sampler2D uMaskTexture;
uniform float uAlpha;

in vec2 fragTexCoord;

void main()
{
    // Pega a cor real da arte do mapa
    vec4 mapColor = texture(uMapTexture, fragTexCoord);

    // Lê a máscara de revelação (Onde o jogador passou é 1.0, onde não passou é 0.0)
    float revealLevel = texture(uMaskTexture, fragTexCoord).r;

    // A cor final é a própria arte
    vec4 finalColor = mapColor;

    // Multiplicamos a transparência da arte pela máscara.
    finalColor.a *= revealLevel;

    // Aplica a transparência global da UI
    finalColor.a *= uAlpha;

    outColor = finalColor;
}