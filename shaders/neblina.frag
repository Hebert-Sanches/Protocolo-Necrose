#version 120

uniform sampler2D uTexture;

varying vec3 vFragPos;
varying vec3 vNormal;

void main() {
    vec4 texColor = texture2D(uTexture, gl_TexCoord[0].st);
    
    vec3 normal = normalize(vNormal);
    vec4 ambient = gl_LightModel.ambient;
    vec4 diffuse = vec4(0.0);
    
    // TRUQUE DE MESTRE: Detecta sozinho se está Indoor ou Outdoor pela luz ambiente!
    if (ambient.r > 0.3) {
        // --- ESTAMOS OUTDOOR (SOL) ---
        // A Luz 0 é direcional
        vec3 lightDir = normalize(gl_LightSource[0].position.xyz);
        float diff = max(dot(normal, lightDir), 0.0);
        diffuse = gl_LightSource[0].diffuse * diff;
    } else {
        // --- ESTAMOS INDOOR (LÂMPADA PISCANDO) ---
        // A Luz 1 tem uma posição exata no mapa
        vec3 lightDir = normalize(gl_LightSource[1].position.xyz - vFragPos);
        float diff = max(dot(normal, lightDir), 0.0);
        
        // Faz a luz ser forte perto e fraca longe (Atenuação)
        float distLuz = length(gl_LightSource[1].position.xyz - vFragPos);
        float atenuacao = 1.0 / (1.0 + 0.1 * distLuz + 0.01 * distLuz * distLuz);
        
        diffuse = gl_LightSource[1].diffuse * diff * atenuacao;
    }
    
    // Junta a textura da parede suja com a luz reconstruída
    vec4 corComLuz = texColor * (ambient + diffuse);
    
    // =====================================
    // A SUA NEBLINA CUSTOMIZADA
    // =====================================
    float distCam = length(vFragPos);
    
    float inicio = 5.0;  // Visão limpa até 5 blocos
    float fim = 35.0;    // Escuridão total a partir de 35 blocos
    
    float fator = clamp((distCam - inicio) / (fim - inicio), 0.0, 1.0);
    
    // Mistura o breu (preto) por cima da parede iluminada
    gl_FragColor = mix(corComLuz, vec4(0.0, 0.0, 0.0, 1.0), fator);
}