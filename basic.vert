#version 330 core
layout (location = 0) in vec2 aPosition;      // Pozicija verteksa
layout (location = 1) in vec2 aTexturePos;    // Teksturne koordinate
layout (location = 1) in vec3 aColor;

out vec2 TexCoord;                           // teksturne koordinate za fragment sejder
out vec3 Color;                              // boja za fragment s

uniform int useTexture;                    // da li koristimo boje ili texturu

uniform mat4 model;                          // Model matrica za ostale objekte
uniform mat4 doorModel;                      // Model matrica za vrata
uniform int isDoor;                          // Da li su verteksi vrata

void main() {

  if (isDoor == 1) {
        gl_Position = doorModel * vec4(aPosition, 0.0, 1.0); // Transformacija samo za vrata
    } else {
        gl_Position = model * vec4(aPosition, 0.0, 1.0);     // Transformacija za ostale objekte
    }
  if (useTexture == 0 || useTexture ==1)
    {
        TexCoord = aTexturePos;
    }else
    {
        TexCoord = vec2(0.0, 0.0); // Prazne koordinate kada ne koristimo teksturu
    }

    Color =aColor;
}
