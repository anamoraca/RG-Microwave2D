#version 330 core
layout (location = 0) in vec2 aPosition;      // Pozicija verteksa
layout (location = 1) in vec2 aTexturePos;    // Teksturne koordinate
layout (location = 1) in vec3 aColor;

out vec2 TexCoord;                           // teksturne koordinate za fragment sejder
out vec3 Color;                              // boja za fragment s

uniform int useTexture;                    // da li koristimo boje ili texturu

void main() {
    gl_Position = vec4(aPosition, 0.0, 1.0);     // Pozicija verteksa u prostoru

  if (useTexture == 0)
    {
        TexCoord = aTexturePos;
    }
    else
    {
        TexCoord = vec2(0.0, 0.0); // Prazne koordinate kada ne koristimo teksturu
    }

    Color =aColor;
}
