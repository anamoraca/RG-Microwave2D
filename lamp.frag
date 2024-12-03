#version 330 core

out vec4 FragColor;
uniform vec3 lampColor;                         // Boja lampice
uniform float pulseFactor;                      // Faktor pulsiranja (0.0 - 1.0)

void main() {
    vec3 color = lampColor * pulseFactor;       // Podesi osvetljenje na osnovu pulsiranja
    FragColor = vec4(color, 1.0);               // Alpha = 1.0 (neprozirno)
}
