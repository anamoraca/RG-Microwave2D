#version 330 core
in vec3 fragColor;                        
out vec4 FragColor;                       // Izlazna boja

uniform float brightness;                 // Trenutni intenzitet svetlosti (pulsiranje)
uniform float opacity;                    // Providnost zraka (0.0 - potpuno providan, 1.0 - potpuno vidljiv)


void main() {
    vec3 color = fragColor * brightness;  
    FragColor = vec4(color, opacity);         
}
