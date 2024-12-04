#version 330 core
out vec4 FragColor;

in vec2 TexCoord;                               // Teksturne koordinate iz vertex s
in vec3 Color;                                  // bojaa iz vertex s

uniform sampler2D ourTexture;                   // Tekstura
uniform int useTexture;


uniform float brightness;                    // Osvetljenje scene


void main() {

    if(useTexture ==0){
        FragColor = texture(ourTexture, TexCoord) *brightness;  // Uzima boju iz teksture
    }else{
        FragColor = vec4(Color, 1.0);
    }

}
