#version 330 core

layout (location = 0) in vec2 aPos;         // Pozicije vrhova
layout (location = 1) in vec3 aColor;       // Boje vrhova

out vec3 vertexColor;                       

uniform int isFoodLamp;                   

void main() {
    gl_Position = vec4(aPos, 0.0, 1.0);     // Pozicija u clip prostoru

    if (isFoodLamp == 1) {
        vertexColor = vec3(1.0, 1.0, 0.6); // Food lamp: svetlo zuta boja
    } else {
        vertexColor = aColor;               // Timer lamp: koristi zadatu boju iz inputa
    }
}
