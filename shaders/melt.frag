#version 120

uniform sampler2D uTex;
uniform float uTimer;
uniform int uXRes;
uniform float uMaxOffset;
uniform float uOffsets[256];

varying vec2 vUV;

void main()
{
    vec2 uv = vUV;

    float col = floor(uv.x * float(uXRes));
    int i = int(clamp(col, 0.0, float(uXRes - 1)));

    float off = uOffsets[i] * uMaxOffset;

    // derrete "para cima" (UV aumenta) => some revelando o fundo
    uv.y -= uTimer * off;

    // quando saiu da área da textura, vira transparente (buraco)
    if (uv.y > 1.0) discard;
    if (uv.y < 0.0) discard;

    gl_FragColor = texture2D(uTex, uv);
}
