#version 330 core

out vec4 FragColor;

in vec2 TexCoord;                               // Teksturne koordinate iz vertex shadera
in vec3 Color;                                  // Boja iz vertex shadera

uniform sampler2D ourTexture;                   // Tekstura 1
uniform sampler2D ourTexture2;                  // Tekstura 2

uniform int useTexture;
uniform float brightness;                       // Osvetljenje scene (lokalno)
uniform int isGlass;                            // Da li crtamo staklo
uniform float transparency;                     // Prozirnost (1.0 = neprozirno, 0.0 = potpuno providno)
uniform float globalBrightness;                 // Globalna osvetljenost (0.0 - tamno, 1.0 - puno svetlo)

void main() {
    vec4 baseColor;

    if (useTexture == 0) {
                                                        // Koristi prvu teksturu
        baseColor = texture(ourTexture, TexCoord);
    } else if (useTexture == 1) {
                                                         // Koristi drugu teksturu
        baseColor = texture(ourTexture2, TexCoord);
    } else {
        if (isGlass == 1) {
                                                    // Poluprovidno staklo sa zadatom providnoscu
            baseColor = vec4(Color, transparency);
        } else {
                                                         // Neprovidni objekti
            baseColor = vec4(Color, 1.0);
        }
    }

    // Primeni globalno osvetljenje na osnovnu boju
    vec3 adjustedColor = baseColor.rgb * globalBrightness;
    FragColor = vec4(adjustedColor, baseColor.a); // Održi providnost
}
