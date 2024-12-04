#version 330 core
layout (location = 0) in vec2 aPos;       // Pozicija verteksa
layout (location = 1) in vec3 aColor;     // Boja verteksa

out vec3 fragColor;                       // Prosledjivanje boje u fragmemt 

void main() {
    gl_Position = vec4(aPos, 0.0, 1.0);   // Transformacija u ekran koordinatni sistem
    fragColor = aColor;                   // Prosle?ivanje boje
}
