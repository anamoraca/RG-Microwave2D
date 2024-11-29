#define _CRT_SECURE_NO_WARNINGS
#define STB_IMAGE_IMPLEMENTATION


#include <direct.h> // Za _getcwd
#include <iostream>
#include <fstream>
#include <sstream>
#include <filesystem>
#include <string>

#include <GL/glew.h>   
#include <GLFW/glfw3.h>
#include "stb_image.h"

unsigned int compileShader(GLenum type, const char* source);
unsigned int createShader(const char* vsSource, const char* fsSource);
unsigned int initVAO(float* vertices, unsigned int* indices, int vertexSize, int indicesSize, int strideV);

int main(void)
{


    if (!glfwInit())
    {
        std::cout << "GLFW Biblioteka se nije ucitala! :(\n";
        return 1;
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    //GLFWmonitor* monitor = glfwGetPrimaryMonitor();                 //trenutni monitor
    //const GLFWvidmode* mode = glfwGetVideoMode(monitor);            //informacije o rezoluciji i osvežavanju
    GLFWwindow* window;
    unsigned int wWidth = 1280; //Podesiti u zavisnosti od rezolucije monitora
    unsigned int wHeight = 720;
  
    const char wTitle[] = "[Generic title]";

    window = glfwCreateWindow(wWidth, wHeight, wTitle, glfwGetPrimaryMonitor(), NULL); //fullscreen

    //window = glfwCreateWindow(mode->width, mode->height, wTitle, monitor, NULL);

    if (window == NULL)
    {
        std::cout << "Prozor nije napravljen! :(\n";
        glfwTerminate();
        return 2;
    }

    glfwMakeContextCurrent(window);                               // radim trenutno na prozoru - window

    if (glewInit() != GLEW_OK)
    {
        std::cout << "GLEW nije mogao da se ucita! :'(\n";
        return 3;
    }

    float counterTopVertices[] = {
        // X       Y       Texture X   Texture Y
         0.4f,  -0.91f,    1.0f,       0.0f,  // Donji desni ugao
         1.0f,   -0.35f,   1.0f,       1.0f,  // Gornji desni ugao
        -1.0f,  -0.89f,    0.0f,       0.0f,  // Donji levi ugao
        -1.0f,   -0.30f,    0.0f,       1.0f   // Gornji levi ugao
    };

    float microwaveVertices[] = {
     0.15f,  -0.67f,   0.6f, 0.6f, 0.6f,  // Donji desni ugao
     0.15f,   0.2f,    0.6f, 0.6f, 0.6f,  // Gornji desni ugao
    -0.65f,  -0.67f,   0.6f, 0.6f, 0.6f,  // Donji levi ugao
    -0.65f,   0.2f,    0.6f, 0.6f, 0.6f   // Gornji levi ugao
    };

    float microwaveSideVertices[] = {
    0.2f , -0.47f ,    0.6f, 0.6f, 0.6f,   //Donji desni  ugao 
    0.2f,   0.3f,    0.6f, 0.6f, 0.6f,  // Gornji desni ugao
    0.15f,  -0.67f,    0.2f, 0.2f, 0.2f,  // Donji levi ugao
    0.15f,   0.2f,     0.2f, 0.2f, 0.2f, // Gornji levi ugao
    };

    float microwaveTopVertices[] = {
    0.15f,  0.2f,    0.2f, 0.2f, 0.2f,   // Donji desni ugao
    0.2f,   0.3f,    0.6f, 0.6f, 0.6f,    // Gornji desni ugao
   -0.65f,  0.2f,    0.2f, 0.2f, 0.2f,   // Donji levi ugao
   -0.60f,  0.3f,    0.6f, 0.6f, 0.6f    //Gordnji levi ugao 

    };


    unsigned int uniIndices[] = {
        0, 1, 2,  // Prvi trougao
        2,3, 1   // Drugi trougao
    };




    unsigned int stride = 5 * sizeof(float);

    unsigned  counterTopVAO, counterTopVBO, counterTopEBO;

    glGenBuffers(1, &counterTopVBO);
    glGenBuffers(1, &counterTopEBO);
    glGenVertexArrays(1, &counterTopVAO);       //  kreira objekte u memoriji GPU

    glBindVertexArray(counterTopVAO);           // trenutni objekat

    glBindBuffer(GL_ARRAY_BUFFER, counterTopVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(counterTopVertices), counterTopVertices, GL_STATIC_DRAW);  // kopira iz RAM u GPU

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, counterTopEBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(uniIndices), uniIndices, GL_STATIC_DRAW);


    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));
    glEnableVertexAttribArray(1);


    unsigned int microwaveVAO, microwaveSideVAO, microwaveTopVAO;

    microwaveVAO = initVAO(microwaveVertices,uniIndices,sizeof(microwaveVertices),sizeof(uniIndices), 5 * sizeof(float));
    microwaveSideVAO =initVAO(microwaveSideVertices, uniIndices,sizeof(microwaveSideVertices), sizeof(uniIndices), 5 * sizeof(float));
    microwaveTopVAO = initVAO(microwaveTopVertices, uniIndices, sizeof(microwaveTopVertices), sizeof(uniIndices), 5 * sizeof(float));


    unsigned int texture;
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);

    // Postavljanje opcija za teksturu
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    stbi_set_flip_vertically_on_load(true); // Flipuje sliku po Y osi

    int width, height, nrChannels;
    unsigned char* data = stbi_load("Textures/wood-texture.jpg",  &width, &height, &nrChannels, 0);
    std::cout << "Ucitavanje teksture uspesno: " << width << "x" << height << ", kanali: " << nrChannels << std::endl;

    if (data) {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
        std::cout << "Uspesno ucitana tekstura: " << width << "x" << height << ", kanali: " << nrChannels << std::endl;
        stbi_image_free(data);
    }
    else {
        std::cout << "Greska pri ucitavanju teksture!" << std::endl;
    }


    unsigned int basicShader = createShader("basic.vert", "basic.frag");

    glClearColor(0.960784, 0.960784, 0.862745, 1.0);

    while (!glfwWindowShouldClose(window))
    {
        glfwPollEvents();
        glClear(GL_COLOR_BUFFER_BIT);

        if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
            glfwSetWindowShouldClose(window, GL_TRUE);
        }

        glUseProgram(basicShader);

        // Use ourTexture uniform for texture binding
        glActiveTexture(GL_TEXTURE0); // Activate texture unit 0
        glBindTexture(GL_TEXTURE_2D, texture); // Bind the texture
        glUniform1i(glGetUniformLocation(basicShader, "ourTexture"), 0); // Texture unit 0


        // Iscrtavanje CounterTop (tekstura)
        glUniform1i(glGetUniformLocation(basicShader, "useTexture"), 0);
        glBindTexture(GL_TEXTURE_2D, texture);

        glBindVertexArray(counterTopVAO);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

        // Iscrtavanje prednjeg dela mikrotalasne, boja
        glUniform1i(glGetUniformLocation(basicShader, "useTexture"), 1);
        glBindVertexArray(microwaveVAO);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

        // microwave side
        glUniform1i(glGetUniformLocation(basicShader, "useTexture"), 1);
        glBindVertexArray(microwaveSideVAO);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

        // microwave top
        glUniform1i(glGetUniformLocation(basicShader, "useTexture"), 1);
        glBindVertexArray(microwaveTopVAO);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

     
        glBindVertexArray(0);
        glUseProgram(0);

        glfwSwapBuffers(window);

    }

    glDeleteVertexArrays(1, &counterTopVAO);
    glDeleteVertexArrays(1, &microwaveVAO);
    glDeleteProgram(basicShader);

    glfwTerminate();
    return 0;
}

unsigned int initVAO(float* vertices, unsigned int* indices,int vertexSize, int indicesSize, int strideV) {
    unsigned int VAO, VBO,EBO;

    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, vertexSize, vertices, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indicesSize, indices, GL_STATIC_DRAW);
    

    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, strideV, (void*)0);  // Pozicija
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, strideV, (void*)(2 * sizeof(float)));  // Boja
    glEnableVertexAttribArray(1);

    glBindVertexArray(0);
    return VAO;
}




















unsigned int compileShader(GLenum type, const char* source)
{
    //Uzima kod u fajlu na putanji "source", kompajlira ga i vraca sejder tipa "type"
    //Citanje izvornog koda iz fajla
    std::string content = "";
    std::ifstream file(source);
    std::stringstream ss;
    if (file.is_open())
    {
        ss << file.rdbuf();
        file.close();
        std::cout << "Uspjesno procitao fajl sa putanje \"" << source << "\"!" << std::endl;
    }
    else {
        ss << "";
        std::cout << "Greska pri citanju fajla sa putanje \"" << source << "\"!" << std::endl;
    }
     std::string temp = ss.str();
     const char* sourceCode = temp.c_str(); //Izvorni kod sejdera koji citamo iz fajla na putanji "source"

    int shader = glCreateShader(type); //Napravimo prazan sejder odredjenog tipa (vertex ili fragment)
    
    int success; //Da li je kompajliranje bilo uspjesno (1 - da)
    char infoLog[512]; //Poruka o gresci (Objasnjava sta je puklo unutar sejdera)
    glShaderSource(shader, 1, &sourceCode, NULL); //Postavi izvorni kod sejdera
    glCompileShader(shader); //Kompajliraj sejder

    glGetShaderiv(shader, GL_COMPILE_STATUS, &success); //Provjeri da li je sejder uspjesno kompajliran
    if (success == GL_FALSE)
    {
        glGetShaderInfoLog(shader, 512, NULL, infoLog); //Pribavi poruku o gresci
        if (type == GL_VERTEX_SHADER)
            printf("VERTEX");
        else if (type == GL_FRAGMENT_SHADER)
            printf("FRAGMENT");
        printf(" sejder ima gresku! Greska: \n");
        printf(infoLog);
    }
    return shader;
}





unsigned int createShader(const char* vsSource, const char* fsSource)
{
    //Pravi objedinjeni sejder program koji se sastoji od Vertex sejdera ciji je kod na putanji vsSource

    unsigned int program; //Objedinjeni sejder
    unsigned int vertexShader; //Verteks sejder (za prostorne podatke)
    unsigned int fragmentShader; //Fragment sejder (za boje, teksture itd)

    program = glCreateProgram(); //Napravi prazan objedinjeni sejder program

    vertexShader = compileShader(GL_VERTEX_SHADER, vsSource); //Napravi i kompajliraj vertex sejder
    fragmentShader = compileShader(GL_FRAGMENT_SHADER, fsSource); //Napravi i kompajliraj fragment sejder

    //Zakaci verteks i fragment sejdere za objedinjeni program
    glAttachShader(program, vertexShader);
    glAttachShader(program, fragmentShader);

    glLinkProgram(program); //Povezi ih u jedan objedinjeni sejder program
    glValidateProgram(program); //Izvrsi provjeru novopecenog programa

    int success;
    char infoLog[512];
    glGetProgramiv(program, GL_VALIDATE_STATUS, &success); //Slicno kao za sejdere
    if (success == GL_FALSE)
    {
        glGetShaderInfoLog(program, 512, NULL, infoLog);
        std::cout << "Objedinjeni sejder ima gresku! Greska: \n";
        std::cout << infoLog << std::endl;
    }

    //Posto su kodovi sejdera u objedinjenom sejderu, oni pojedinacni programi nam ne trebaju, pa ih brisemo zarad ustede na memoriji
    glDetachShader(program, vertexShader);
    glDeleteShader(vertexShader);
    glDetachShader(program, fragmentShader);
    glDeleteShader(fragmentShader);

    return program;
}
