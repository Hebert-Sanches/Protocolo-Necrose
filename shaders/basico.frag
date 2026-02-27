#version 330 core

out vec4 FragColor;

in vec2 TexCoord;

uniform sampler2D texture1;

void main() {
    // Define a cor baseada no pixel correspondente da textura
    FragColor = texture(texture1, TexCoord);
}