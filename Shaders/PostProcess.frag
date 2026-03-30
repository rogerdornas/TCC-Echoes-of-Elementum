#version 330 core
out vec4 FragColor;
in vec2 TexCoord;

uniform sampler2D uScreenTexture;
uniform int uEffectType;      // 0 = Normal, 1 = Blur, 2 = Grayscale
uniform float uEffectIntensity; // Quão forte é o desfoque
uniform vec2 uResolution;     // Tamanho da tela para calcular o tamanho de 1 pixel

void main()
{
    vec4 texColor = texture(uScreenTexture, TexCoord);

    // 0: Nenhum Efeito
    if (uEffectType == 0) {
        FragColor = texColor;
    }
    // 1: BLUR (Desfoque Gaussiano simples de 9 amostras)
    else if (uEffectType == 1) {
        vec2 texOffset = 1.0 / uResolution;
        vec4 blurColor = vec4(0.0);

        // Coleta pixels em um raio maior e mais suave
        int blurRadius = 5; // Quanto maior, mais forte/suave o blur inicial
        float totalWeight = 0.0;

        for(int x = -blurRadius; x <= blurRadius; x++) {
            for(int y = -blurRadius; y <= blurRadius; y++) {
                vec2 offset = vec2(float(x), float(y)) * texOffset * uEffectIntensity;

                // Pesa mais os pixels do centro e menos os da borda (Gaussiano falso)
                float weight = 1.0 - (length(vec2(x, y)) / float(blurRadius));
                if (weight > 0.0) {
                    blurColor += texture(uScreenTexture, TexCoord + offset) * weight;
                    totalWeight += weight;
                }
            }
        }
        FragColor = blurColor / totalWeight;
    }
    // 2: GRAYSCALE (Preto e branco)
    else if (uEffectType == 2) {
        // Fórmula padrão de luminância para os olhos humanos
        float gray = dot(texColor.rgb, vec3(0.299, 0.587, 0.114));
        FragColor = vec4(gray, gray, gray, texColor.a);
    }
    // 3: DAMAGE FLASH (Piscar Vermelho de Dano)
    else if (uEffectType == 3) {
        // 1. Calcula a distância do pixel até o centro da tela (0.5, 0.5)
        // Isso nos dá um valor de 0.0 no centro e ~0.7 nas pontas
        vec2 center = TexCoord - vec2(0.5);
        float dist = length(center);

        // 2. Cor do Sangue/Dano
        vec4 damageColor = vec4(0.3, 0.1, 0.1, 0.8); // Vermelho

        // 3. Fator de mistura (Mix Factor)
        // Pega a intensidade mandada pelo C++ (0.0 a 1.0)
        // Adiciona a distância para que as bordas fiquem mais vermelhas que o centro
        float mixFactor = uEffectIntensity * (0.2 + dist);

        // Garante que a mistura não passe de 100%
        mixFactor = clamp(mixFactor, 0.0, 1.0);

        // 4. Mistura a cor original do jogo com a cor de dano
        FragColor = mix(texColor, damageColor, mixFactor);
    }
}