#version 330 core

// Atributos de entrada (precisam bater com o seu Vertex Array Object no C++)
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aTexCoord;

// Variáveis de saída para o Fragment Shader
out vec2 TexCoord;

// Matrizes de transformação (enviadas do C++)
uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main() {
    // Multiplicação de matrizes: Projeção * Câmera * Objeto * Posição do Vértice
    gl_Position = projection * view * model * vec4(aPos, 1.0);
    
    // Repassando a coordenada de textura
    TexCoord = aTexCoord;
}