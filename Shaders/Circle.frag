// Request GLSL 3.3
#version 330

in vec2 fragLocalPos;
out vec4 outColor;

uniform float uRatio;      // Porcentagem da barra (0.0 a 1.0)
uniform vec3 uFillColor;   // Cor da parte cheia
uniform vec3 uBgColor;     // Cor do fundo do anel
uniform float uAlpha;      // Opacidade global
uniform float uThickness;  // Espessura normalizada (ex: 0.2 significa 20% do raio)

const float PI = 3.14159265359;

void main()
{
    // fragLocalPos vai de -0.5 a 0.5 (porque seu mSpriteVerts tem esse tamanho).
    // Multiplicamos por 2 para mapear para -1.0 a 1.0 (facilita a matemática).
    vec2 uv = fragLocalPos * 2.0;

    // Distância do pixel até o centro (0,0)
    float dist = length(uv);

    // O raio interno é 1.0 menos a espessura
    float innerRadius = 1.0 - uThickness;

    // Smoothstep cria um anti-aliasing perfeito (bordas suaves em vez de serrilhadas)
    float smoothing = 0.015;
    float outerEdge = smoothstep(1.0, 1.0 - smoothing, dist);
    float innerEdge = smoothstep(innerRadius - smoothing, innerRadius, dist);

    // Multiplicar as duas bordas cria a máscara do anel
    float ringMask = outerEdge * innerEdge;

    // Se estiver fora do anel, descarta o pixel imediatamente (transparente)
    if (ringMask < 0.01) {
        discard;
    }

    // --- CÁLCULO DO ÂNGULO ---
    // atan(y, x) retorna de -PI a PI.
    float angle = atan(uv.y, uv.x);

    // Ajusta o ângulo para que 0 seja no topo (12 horas) e gire no sentido horário
    // Como a tela no SDL/OpenGL tem o Y invertido em 2D, a matemática fica assim:
    float normalizedAngle = mod(angle + (PI / 2.0), 2.0 * PI) / (2.0 * PI);

    // Se o ângulo do pixel for menor que o ratio, ele faz parte do preenchimento
    // smoothstep aqui suaviza levemente a ponta da barra de progresso
    float fillMask = 1.0 - smoothstep(uRatio - 0.005, uRatio + 0.005, normalizedAngle);

    // Mistura a cor de fundo com a cor de preenchimento baseada na máscara
    vec3 finalColor = mix(uBgColor, uFillColor, fillMask);

    // Aplica a cor com a transparência global e o anti-aliasing da borda
    outColor = vec4(finalColor, uAlpha * ringMask);
}