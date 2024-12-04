#version 330 core
out vec4 FragColor;

in vec3 outColor;                                    
in vec2 Position;                                   

uniform vec2 center;   // Centar elipse
uniform vec2 scale;    // Skala elipse (širina, visina)
uniform float time;    // Prolazak vremena (za animaciju)
uniform float opacity; // Maksimalna providnost oblaka

void main() {

                                         // Normalizovana pozicija oblaka u odnosu na centar

    vec2 normPos = (Position - center) / scale;

                                             // Elipsa: x^2 / a^2 + y^2 / b^2 <= 1

    float distance = dot(normPos, normPos);
        
                                                         // Ako smo unutar elipse, renderuj dim

    if (distance <= 1.0) {

                                                        // Radijalna providnost (udaljenost bliza ivici = providnije)

        float radialOpacity = 1.0 - distance;

                                                                 // Animacija providnosti u vremenu

        float animatedOpacity = opacity * abs(sin(time)) * radialOpacity;

                                                                // Finalna boja oblaka

        FragColor = vec4(outColor, animatedOpacity);
    } else {
                                                            // Ako smo izvan elipse, nista ne iscrtavaj
        discard;
    }
}
