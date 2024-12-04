#version 330 core

out vec4 FragColor;

uniform vec3 lampColor;                     // Boja lampe
uniform float pulseFactor;                  // Faktor pulsiranja (samo za timer lampu)
uniform int isFoodLamp;                    // Da li je food lamp

uniform float pulseFactor2; 

void main() {
    vec3 finalColor;

    if (isFoodLamp == 1) {
        
        finalColor = lampColor*pulseFactor2;             
    } else {
        
        finalColor = lampColor *pulseFactor;
    }

    FragColor = vec4(finalColor, 1.0);      // Alpha = 1.0 (neprozirno)
}
