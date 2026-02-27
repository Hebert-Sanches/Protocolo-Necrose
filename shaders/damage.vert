#version 120
varying vec2 vUV;
void main() {
    gl_Position = gl_ModelViewProjectionMatrix * gl_Vertex;
    vUV = gl_MultiTexCoord0.xy;
}