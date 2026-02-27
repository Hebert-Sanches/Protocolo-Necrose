#version 120

uniform float uTime;
varying vec2 vUV;

void main() {
    // Distância do centro
    float dist = distance(vUV, vec2(0.5, 0.5));
    
    // Suaviza a borda (começa a aparecer nos 30% da tela e fica forte nas pontas)
    float vignette = smoothstep(0.3, 0.8, dist);
    
    // Pulsação suave simulando o coração (vai de 0.0 a 1.0)
    float pulso = (sin(uTime * 5.0) + 1.0) * 0.5; 
    
    // Transparência: mistura a borda com o pulso
    float alpha = vignette * (0.3 + pulso * 0.5);
    
    // Vermelho sangue
    gl_FragColor = vec4(0.6, 0.0, 0.0, alpha);
}