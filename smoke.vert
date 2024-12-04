#version 330 core
layout(location = 0) in vec2 aPos;         // Pozicija verteksa
layout(location = 1) in vec3 Color;        // Boja verteksa

out vec3 outColor;                        // Proslejivanje boje u fragment sader
out vec2 Position;                        // Prosleivanje pozicije u fragment saader

void main() {
    gl_Position = vec4(aPos, 0.0, 1.0);   // Pozicija u prostoru
    outColor = Color;                     // Prosleivanje boje
    Position = aPos;                      // Prosleivanje pozicije
}
