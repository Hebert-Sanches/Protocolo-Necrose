#version 120

varying vec3 vFragPos;
varying vec3 vNormal;

void main() {
    // Calcula a posição na tela
    gl_Position = gl_ModelViewProjectionMatrix * gl_Vertex;
    
    // Calcula a posição da parede no mundo 3D
    vFragPos = vec3(gl_ModelViewMatrix * gl_Vertex);
    
    // Calcula a direção da parede (Normal) para a luz bater certo
    vNormal = normalize(gl_NormalMatrix * gl_Normal);
    
    // Passa a coordenada da textura
    gl_TexCoord[0] = gl_MultiTexCoord0;
}