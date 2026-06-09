#version 330
out vec4 outColor;
in vec2 fragTexCoord;

uniform sampler2D uTexture;

void main()
{
    vec4 texColor = texture(uTexture, fragTexCoord);

    // Multiplica o canal Vermelho pelo canal Alpha.
    // Isso garante que o gradiente do blur exista na COR pura,
    // fazendo o GL_MAX funcionar perfeitamente com qualquer PNG.
    float intensity = texColor.r * texColor.a;

    outColor = vec4(intensity, 0.0, 0.0, 1.0);
}