#version 330 core

layout (location = 0) in vec2 aPos;         // Pozicije vrhova
layout (location = 1) in vec3 aColor;       // Boje vrhova

out vec3 vertexColor;                       // Prosledjuje boju fragment shaderu

void main() {
    gl_Position = vec4(aPos, 0.0, 1.0);     // Pozicija u clip prostoru
    vertexColor = aColor;               // Prosledjivanje boje
}
