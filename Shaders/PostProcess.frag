#version 330 core
out vec4 FragColor;
in vec2 TexCoord;

uniform sampler2D uScreenTexture;
uniform vec2 uResolution;

uniform float uBlurIntensity;
uniform float uGrayscaleIntensity;
uniform float uDamageIntensity;
uniform float uAberrationIntensity;
uniform float uAberrationAngle;

void main()
{
    vec4 baseColor = texture(uScreenTexture, TexCoord);

    // EFEITOS ESPACIAIS
    if (uBlurIntensity > 0.0) {
        vec2 texOffset = 1.0 / uResolution;
        vec4 blurColor = vec4(0.0);
        int blurRadius = 5;
        float totalWeight = 0.0;

        for(int x = -blurRadius; x <= blurRadius; x++) {
            for(int y = -blurRadius; y <= blurRadius; y++) {
                vec2 offset = vec2(float(x), float(y)) * texOffset * uBlurIntensity;
                float weight = 1.0 - (length(vec2(x, y)) / float(blurRadius));
                if (weight > 0.0) {
                    blurColor += texture(uScreenTexture, TexCoord + offset) * weight;
                    totalWeight += weight;
                }
            }
        }
        baseColor = blurColor / totalWeight;
    }
    else if (uAberrationIntensity > 0.0) {
        float maxShift = 0.020;
        float shiftAmount = maxShift * uAberrationIntensity;

        // Calcula ângulos defasados em 120 graus (2.094 radianos) para cada cor
        float angleR = uAberrationAngle;
        float angleG = uAberrationAngle + 2.09439;
        float angleB = uAberrationAngle + 4.18879;

        // Converte os ângulos em vetores de direção (Trigonometria básica)
        vec2 dirR = vec2(cos(angleR), sin(angleR));
        vec2 dirG = vec2(cos(angleG), sin(angleG));
        vec2 dirB = vec2(cos(angleB), sin(angleB));

        // Aplica o deslocamento empurrando cada cor para sua própria direção
        float r = texture(uScreenTexture, TexCoord + (dirR * shiftAmount)).r;
        float g = texture(uScreenTexture, TexCoord + (dirG * shiftAmount)).g;
        float b = texture(uScreenTexture, TexCoord + (dirB * shiftAmount)).b;

        // Substitui a cor base
        baseColor = vec4(r, g, b, baseColor.a);
    }

    // EFEITOS DE COR

    // Grayscale (Mistura a cor atual com a versão cinza dela mesma)
    if (uGrayscaleIntensity > 0.0) {
        float gray = dot(baseColor.rgb, vec3(0.299, 0.587, 0.114));
        vec4 grayColor = vec4(gray, gray, gray, baseColor.a);

        baseColor = mix(baseColor, grayColor, uGrayscaleIntensity);
    }

    // Damage Flash (Mistura o vermelho do dano por cima de tudo)
    if (uDamageIntensity > 0.0) {
        vec2 center = TexCoord - vec2(0.5);
        float dist = length(center);
        vec4 damageColor = vec4(0.3, 0.1, 0.1, 0.8);

        float mixFactor = uDamageIntensity * (0.2 + dist);
        mixFactor = clamp(mixFactor, 0.0, 1.0);

        baseColor = mix(baseColor, damageColor, mixFactor);
    }

    // Resultado final
    FragColor = baseColor;
}