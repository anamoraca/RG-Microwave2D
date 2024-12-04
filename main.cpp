    #define _CRT_SECURE_NO_WARNINGS
    #define STB_IMAGE_IMPLEMENTATION

    #include <direct.h> // Za _getcwd
    #include <iostream>
    #include <fstream>
    #include <sstream>
    #include <filesystem>
    #include <string>

    #include <ft2build.h>
    #include FT_FREETYPE_H
    #include <GL/glew.h>   
    #include <GLFW/glfw3.h>
    #include "stb_image.h"
    #include <map>
    #include <string>
    #include <iostream>
    #include <glm/glm.hpp>
    #include <glm/gtc/matrix_transform.hpp>

    #include <glm/glm.hpp>
    #include <glm/gtc/matrix_transform.hpp>
    #include <glm/gtc/type_ptr.hpp>

    bool isRunning = false;                
    int minutes = 0, seconds = 0;         
    std::string curentTime = "00:00";
    float numberButtonVertices[10][20];    
    int inputCount = 0;                    //brojac klk je puta kliknuo

    bool isLampOn = true;              
    bool isPulsing = false;                
    float lampPulse = 0.0f;                // vrednost za pulsiranje (0.0 - 1.0)
    float lampFoodPulse = 0.0f;
    bool increasing = false;

    bool isBroken = false;               
    float sceneBrightness = 1.0f;         // osvetljenost scene (0.0 - tamno, 1.0 - svetlo)
    bool isTextBlinking = false;          
    double lastBlinkTime = 0.0;         

    glm::vec2 smokeStart = glm::vec2(0.25f, 0.2f); // Gornji desni ugao mikrotalasne
    glm::vec2 smokeCenter = smokeStart;            // Trenutna pozicija dima
    glm::vec2 smokeScale = glm::vec2(0.3f, 0.6f);  // Skala elipse
    float maxOpacity = 0.5f;                       // Maksimalna providnost
    float dimTime = 0.0f;                          // Vremenska varijabla za animaciju
    bool isSmokeVisible = false;     

    float doorTransparency = 0.5f;  // Početna vrednost: neprozirno
    float globalBrightness = 1.0f;  // Početna vrednost osvetljenja: 100% (potpuno svetlo)


    bool isDoorOpen = false;
        
    struct Character { 
        unsigned int TextureID;           // id teksture
        glm::ivec2 Size;                  // Velicina glifa
        glm::ivec2 Bearing;               // Offset od osnovne linije
        unsigned int Advance;             // Razmak do sledećeg glifa
    };

    std::map<char, Character> Characters;

    unsigned int compileShader(GLenum type, const char* source);
    unsigned int createShader(const char* vsSource, const char* fsSource);
    unsigned int initVAO(float* vertices, unsigned int* indices, int vertexSize, int indicesSize, int strideV);
    void loadFont(const char* fontPath);
    void RenderText(unsigned int shader, const std::string& text, float x, float y, float scale, glm::vec3 color, unsigned int& VAO, unsigned int& VBO);
   
    int detectButtonClick(double xpos, double ypos);

    void handleKeyPress(int number);
    void handleButtonPress(int buttonNumber);

    void updateLamp(); 
    void updateTimer();
    void resetTimer();
    void startTimer();
    void stopTimer();

    void repairMicrowave();
    void updateBrokenState();

    // Funkcija za detekciju miša
    void mouse_button_callback(GLFWwindow* window, int button, int action, int mods) {
        if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS) {
            double xpos, ypos;
            glfwGetCursorPos(window, &xpos, &ypos);

            GLFWmonitor* monitor = glfwGetPrimaryMonitor();                 //trenutni monitor
            const GLFWvidmode* mode = glfwGetVideoMode(monitor);
          
            // Normalizacija koordinata
            xpos = (xpos / (double)mode->width) * 2.0 - 1.0;               // Normalizacija X
            ypos = 1.0 - (ypos / (double)mode->height) * 2.0;              // Obrnuta i normalizovana Y

            // Provera koji je taster kliknut
            int buttonPressed = detectButtonClick(xpos, ypos);

            if (buttonPressed>-1 && buttonPressed<10) {
                handleKeyPress(buttonPressed);
            }
            else if(buttonPressed >9 && buttonPressed < 13){
                handleButtonPress(buttonPressed);
            }
            else {
                std::cout << "No button pressed!" << std::endl;
            }
        }
    }

    void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {
        if (action == GLFW_PRESS) {
            if (key == GLFW_KEY_X) {  //  kvar
                isBroken = true;
                isRunning = false;
                isLampOn = false;
                isPulsing = false;
                std::cout << "Microwave broken!" << std::endl;

                globalBrightness -= 0.4f;  // Smanji osvetljenje za 10%
                if (globalBrightness < 0.4f) globalBrightness = 0.4f; // Minimalno osvetljenje 10%
                std::cout << "Global brightness: " << globalBrightness << std::endl;

            }
            if (key == GLFW_KEY_R) {  // T servisirane
                repairMicrowave();

                globalBrightness += 0.4f;  // Povećaj osvetljenje za 10%
                if (globalBrightness > 1.0f) globalBrightness = 1.0f; // Maksimalno osvetljenje 100%
                std::cout << "Global brightness: " << globalBrightness << std::endl;

                std::cout << "Microwave repaired!" << std::endl;
            }
            else if (key == GLFW_KEY_O) { 
                if(!isBroken)
                isDoorOpen = true;
                isRunning = false; 
                std::cout << "Microwave door opened." << std::endl;
            }
            else if (key == GLFW_KEY_Z) {  // zatvara vrata
                isDoorOpen = false;
                std::cout << "Microwave door closed." << std::endl;
            }
            else if (key == GLFW_KEY_D) {  // Dugme `D` menja prozirnost
                doorTransparency = (doorTransparency == 1.0f) ? 0.5f : 1.0f;  // Prebaci između 50% i 100%
                std::cout << "Door transparency toggled to: " << doorTransparency << std::endl;
            }
        }
    }

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

        GLFWmonitor* monitor = glfwGetPrimaryMonitor();                 //trenutni monitor
        const GLFWvidmode* mode = glfwGetVideoMode(monitor);            //informacije o rezoluciji i osvežavanju
        GLFWwindow* window;
  
        const char wTitle[] = "Microwave";

       // window = glfwCreateWindow(wWidth, wHeight, wTitle, glfwGetPrimaryMonitor(), NULL); //fullscreen

        window = glfwCreateWindow(mode->width, mode->height, wTitle, monitor, NULL);

        if (window == NULL)
        {
            std::cout << "Prozor nije napravljen! :(\n";
            glfwTerminate();
            return 2;
        }

        glfwMakeContextCurrent(window);                               // radim trenutno na prozoru - window
        glfwSwapInterval(1);                    // Uključuje sinhronizaciju sa vertikalnim osvežavanjem monitora

        // Inicijalizacija miša
        glfwSetMouseButtonCallback(window, mouse_button_callback);
        glfwSetKeyCallback(window, key_callback);

        if (glewInit() != GLEW_OK)
        {
            std::cout << "GLEW nije mogao da se ucita! :'(\n";
            return 3;
        }

        float counterTopVertices[] = {
            // X       Y       Texture X   Texture Y
             1.0f,  -0.91f,    1.0f,       0.0f,  // Donji desni ugao
             1.0f,   -0.35f,   1.0f,       1.0f,  // Gornji desni ugao
            -1.0f,  -0.89f,    0.0f,       0.0f,  // Donji levi ugao
            -1.0f,   -0.30f,    0.0f,       1.0f   // Gornji levi ugao
        };

        float pictureVertices[] = {
         0.8f,  0.7f,   0.6f, 0.6f, 0.6f,  // Donji desni ugao
         0.8f,   0.9f,    0.6f, 0.6f, 0.6f,  // Gornji desni ugao
         0.5f,  0.7f,   0.6f, 0.6f, 0.6f,  // Donji levi ugao
         0.5f,   0.9f,    0.6f, 0.6f, 0.6f   // Gornji levi ugao
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

        float doorFrameVertices[] = {
            -0.13f,  -0.67f,   0.0f, 0.0f, 0.0f,  // Donji desni ugao
            -0.13f,   0.2f,    0.0f, 0.0f, 0.0f,  // Gornji desni ugao
           -0.65f,  -0.67f,   0.0f, 0.0f, 0.0f, // Donji levi ugao
           -0.65f,   0.2f,    0.0f, 0.0f, 0.0f   // Gornji levi ugao
        };

        float doorGlassVertices[] = {
          -0.18f,  -0.60f,   0.5f, 0.7f, 1.0f,   // Donji desni ugao
          -0.18f,   0.13f,  0.5f, 0.7f, 1.0f,     // Gornji desni ugao
          -0.60f,  -0.60f,   0.5f, 0.7f, 1.0f,    // Donji levi ugao
          -0.60f,   0.13f,    0.5f, 0.7f, 1.0f   // Gornji levi ugao
        };

        float microwaveInsideVertices[] = {
          -0.18f,  -0.60f,   1.0f, 1.0f, 1.0f,      // Donji desni ugao
          -0.18f,   0.13f,    1.0f, 1.0f, 1.0f,          // Gornji desni ugao
          -0.60f,  -0.60f,   1.0f, 1.0f, 1.0f,     // Donji levi ugao
          -0.60f,   0.13f,   1.0f, 1.0f, 1.0f        // Gornji levi ugao
        };

        float foodVertices[] = {
         -0.2f,  -0.60f,   1.0f, 0.0f,      // Donji desni ugao
         -0.2f,   -0.2f,   1.0f,  1.0f,          // Gornji desni ugao
         -0.58f,  -0.60f,   0.0f, 0.0f,     // Donji levi ugao
         -0.58f,   -0.2f,   0.0f, 1.0f        // Gornji levi ugao
        };

        float foodLampVertices[] = {
        -0.35f,  0.1f,  1.0f, 1.0f, 0.5f,  // Donji desni ugao
        -0.35f,  0.13f, 1.0f, 1.0f, 0.5f,  // Gornji desni ugao
        -0.42f,  0.1f,  1.0f, 1.0f, 0.5f,  // Donji levi ugao
        -0.42f,  0.13f, 1.0f, 1.0f, 0.5f   // Gornji levi ugao
        };

        float foodLamplightVertices[] = {
       -0.18f,  -0.60f, 1.0f, 1.0f, 0.6f,  // Donji desni ugao
       -0.35f,  0.13f, 1.0f, 1.0f, 0.6f, // Gornji desni ugao
      -0.60f,  -0.60f,   0.5f, 0.5f, 0.3f,  // Donji levi ugao
       -0.42f,  0.13f,  0.5f, 0.5f, 0.3f,   // Gornji levi ugao
        };


        float lampVertices[]{
            0.12, 0.14,  0.0f, 0.0f, 0.0f,       //donji desni 
            0.12,  0.185,  0.0f, 0.0f, 0.0f,    // gornji desni 
            -0.1, 0.14,   0.0f, 0.0f, 0.0f,     //donji levi
           -0.1,  0.185,    0.0f, 0.0f, 0.0f     //gornji levi
        };


        float lampLineVertices[]{
            0.11,  0.15,     1.0f, 1.0f, 1.0f,       //donji desni 
            0.11,  0.175,  1.0f, 1.0f, 1.0f,    // gornji desni 
           -0.09, 0.15,    1.0f, 1.0f, 1.0f,      //donji levi
           -0.09, 0.175,     1.0f, 1.0f, 1.0f     //gornji levi
        };

        float keyboardFrameVertices[] = {
            0.12, -0.6,  0.2f, 0.2f, 0.2f,       //donji desni 
            0.12,  0.13,    0.2f, 0.2f, 0.2f,    // gornji desni 
           -0.1, -0.6,  0.2f, 0.2f, 0.2f,      //donji levi
           -0.1, 0.13,    0.2f, 0.2f, 0.2f     //gornji levi 
        };

        float timerVertices[] = {
            0.1f,  0.1f,  0.9f, 0.9f, 0.9f,  // Gornji desni ugao
            0.1f,  -0.03f,   0.9f, 0.9f, 0.9f,  //Donji desni ugao
           -0.08f,  0.1f,  0.9f, 0.9f, 0.9f,  // Gornji levi ugao
           -0.08f,  -0.03f,   0.9f, 0.9f, 0.9f   // Donji levi ugao
        };

        float keyboardVertices[] = {
            0.1f,  -0.36f,  0.7f, 0.7f, 0.7f,  // Donji desni ugao
            0.1f,   -0.06, 0.7f, 0.7f, 0.7f,    // Gornji desni ugao
           -0.08f,   -0.36f,  0.7f, 0.7f, 0.7f,    // Donji levi ugao
           -0.08f,   -0.06, 0.7f, 0.7f, 0.7f,    // Gornji levi ugao
        };

        float startStopButtonVertices[] = {
            0.1f,  -0.46f,   0.7f, 0.7f, 0.7f,  // Donji desni ugao
            0.1f,  -0.38f,   0.7f, 0.7f, 0.7f,  // Gornji desni ugao
           -0.08f,  -0.46f,   0.7f, 0.7f, 0.7f,  // Donji levi ugao
           -0.08f,  -0.38f,   0.7f, 0.7f, 0.7f   // Gornji levi ugao
        };

        float restartButtonVertices[] = {
             0.1f,  -0.57f,   0.7f, 0.7f, 0.7f,  // Donji desni ugao
             0.1f,  -0.48f,   0.7f, 0.7f, 0.7f,  // Gornji desni ugao
            -0.08f,  -0.57f,   0.7f, 0.7f, 0.7f,  // Donji levi ugao
            -0.08f,  -0.48f,   0.7f, 0.7f, 0.7f   // Gornji levi ugao
        };

        float smokeVertices[] = {
          -1.0f, -1.0f, 0.1f, 0.1f, 0.1f,  // Donji levi ugao
          -1.0f,  1.0f, 0.2f, 0.2f, 0.2f,  // Gornji levi ugao
           1.0f, -1.0f, 0.0f, 0.0f, 0.0f,  // Donji desni ugao
           1.0f,  1.0f, 0.8f, 0.8f, 0.8f   // Gornji desni ugao
        };


        unsigned int uniIndices[] = {
            0, 1, 2,                        // Prvi trougao
            2,3, 1                          // Drugi trougao
        };

        float buttonWidth = 0.04f;          // sirina dugmeta
        float buttonHeight = 0.05f;         // Visina dugmeta
        float xOffset = -0.07f;             // horizontalni pomak (od leve ivice tastature)
        float yOffset = -0.13f;             // vertikalni pomak (od gornje ivice tastature)

        // Niz za dugmice 0-9
        float numberButtonVertices[10][20];

        for (int i = 0; i < 10; ++i) {
            int row = i / 3;           
            int col = i % 3; 
            if (i == 9) {    
                row = 3;
                col = 1;
            }
            float xStart = xOffset + col * (buttonWidth + 0.02f); // X koordinata
            float yStart = yOffset - row * (buttonHeight + 0.02f); // Y koordinata

            // Definisi pravougaonik za dugme
            numberButtonVertices[i][0] = xStart + buttonWidth;  // Donji desni ugao
            numberButtonVertices[i][1] = yStart;
            numberButtonVertices[i][2] = 1.0f; // Boja (siva)
            numberButtonVertices[i][3] = 1.0f;
            numberButtonVertices[i][4] = 1.0f;

            numberButtonVertices[i][5] = xStart + buttonWidth;  // Gornji desni ugao
            numberButtonVertices[i][6] = yStart + buttonHeight;
            numberButtonVertices[i][7] = 0.0f;
            numberButtonVertices[i][8] = 0.0f;
            numberButtonVertices[i][9] = 0.0f;

            numberButtonVertices[i][10] = xStart;               // Donji levi ugao
            numberButtonVertices[i][11] = yStart;
            numberButtonVertices[i][12] = 0.0f;
            numberButtonVertices[i][13] = 0.0f;
            numberButtonVertices[i][14] = 0.0f;

            numberButtonVertices[i][15] = xStart;               // Gornji levi ugao
            numberButtonVertices[i][16] = yStart + buttonHeight;
            numberButtonVertices[i][17] = 0.0f;
            numberButtonVertices[i][18] = 0.0f;
            numberButtonVertices[i][19] = 0.0f;

            std::cout << "Button " << i << " coordinates: "
                      << "xStart=" << xStart << ", xEnd=" << (xStart + buttonWidth)
                      << ", yStart=" << yStart << ", yEnd=" << (yStart + buttonHeight) << std::endl;

        }

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


        unsigned  foodVAO, foodVBO, foodEBO;

        glGenBuffers(1, &foodVBO);
        glGenBuffers(1, &foodEBO);
        glGenVertexArrays(1, &foodVAO);       //  kreira objekte u memoriji GPU

        glBindVertexArray(foodVAO);           // trenutni objekat
        glBindBuffer(GL_ARRAY_BUFFER, foodVBO);

        glBufferData(GL_ARRAY_BUFFER, sizeof(foodVertices), foodVertices, GL_STATIC_DRAW);  // kopira iz RAM u GPU

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, foodEBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(uniIndices), uniIndices, GL_STATIC_DRAW);

        glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(0);

        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));
        glEnableVertexAttribArray(1);

        unsigned int microwaveVAO, microwaveSideVAO, microwaveTopVAO,
                     keyboardFrameVAO, timerVAO, keyboardVAO,buttonVAO, startStopButtonVAO,
                     restartButtonVAO, doorFrameVAO, doorGlassVAO,pictureVAO, microwaveInsideVAO,
                     lampVAO, lampLineVAO,smokeVAO, foodLampVAO, foodLampLightVAO;

        microwaveVAO = initVAO(microwaveVertices,uniIndices,sizeof(microwaveVertices),sizeof(uniIndices), 5 * sizeof(float));
        microwaveSideVAO =initVAO(microwaveSideVertices, uniIndices,sizeof(microwaveSideVertices), sizeof(uniIndices), 5 * sizeof(float));
        microwaveTopVAO = initVAO(microwaveTopVertices, uniIndices, sizeof(microwaveTopVertices), sizeof(uniIndices), 5 * sizeof(float));
        keyboardFrameVAO = initVAO(keyboardFrameVertices, uniIndices, sizeof(keyboardFrameVertices), sizeof(uniIndices), 5 * sizeof(float));
        timerVAO = initVAO(timerVertices, uniIndices, sizeof(timerVertices), sizeof(uniIndices), 5 * sizeof(float));
        keyboardVAO = initVAO(keyboardVertices, uniIndices, sizeof(keyboardVertices), sizeof(uniIndices), 5 * sizeof(float));
        startStopButtonVAO = initVAO(startStopButtonVertices, uniIndices, sizeof(startStopButtonVertices), sizeof(uniIndices), 5 * sizeof(float));
        restartButtonVAO =  initVAO(restartButtonVertices, uniIndices, sizeof(restartButtonVertices), sizeof(uniIndices), 5 * sizeof(float));
        doorFrameVAO = initVAO(doorFrameVertices, uniIndices, sizeof(doorFrameVertices), sizeof(uniIndices), 5 * sizeof(float));
        doorGlassVAO = initVAO(doorGlassVertices, uniIndices, sizeof(doorGlassVertices), sizeof(uniIndices), 5 * sizeof(float));
        pictureVAO= initVAO(pictureVertices, uniIndices, sizeof(pictureVertices), sizeof(uniIndices), 5 * sizeof(float));
        microwaveInsideVAO = initVAO(microwaveInsideVertices, uniIndices, sizeof(microwaveInsideVertices), sizeof(uniIndices), 5 * sizeof(float));
        lampVAO = initVAO(lampVertices, uniIndices, sizeof(lampVertices), sizeof(uniIndices), 5 * sizeof(float));
        lampLineVAO = initVAO(lampLineVertices, uniIndices, sizeof(lampLineVertices), sizeof(uniIndices), 5 * sizeof(float));
        smokeVAO = initVAO(smokeVertices, uniIndices, sizeof(smokeVertices), sizeof(uniIndices), 5 * sizeof(float));
        foodLampVAO = initVAO(foodLampVertices, uniIndices, sizeof(foodLampVertices), sizeof(uniIndices), 5 * sizeof(float));
        foodLampLightVAO = initVAO(foodLamplightVertices, uniIndices, sizeof(foodLamplightVertices), sizeof(uniIndices), 5 * sizeof(float));

        
        // TEXT

        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

        unsigned int textVAO, textVBO;
            glGenVertexArrays(1, &textVAO);
            glGenBuffers(1, &textVBO);
            glBindVertexArray(textVAO);
            glBindBuffer(GL_ARRAY_BUFFER, textVBO);
            glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 6 * 4, NULL, GL_DYNAMIC_DRAW);
            glEnableVertexAttribArray(0);
            glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), 0);
            glBindBuffer(GL_ARRAY_BUFFER, 0);
            glBindVertexArray(0);

            loadFont("ARIAL.TTF");

    // TEXTURE
        unsigned int texture, foodTexture;
        glGenTextures(1, &texture);
        glBindTexture(GL_TEXTURE_2D, texture);

        int width, height, nrChannels;
        unsigned char* data = stbi_load("Textures/wood-texture.jpg",  &width, &height, &nrChannels, 0);

        if (data) {
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
            glGenerateMipmap(GL_TEXTURE_2D);
            stbi_image_free(data);
        }
        else {
            std::cout << "Greska pri ucitavanju teksture!" << std::endl;
        }

        glGenTextures(1, &foodTexture);
        glBindTexture(GL_TEXTURE_2D, foodTexture);
        stbi_set_flip_vertically_on_load(true);
        unsigned char* data2 = stbi_load("Textures/food.jpg", &width, &height, &nrChannels, 0);

        if (data2) {
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data2);
            glGenerateMipmap(GL_TEXTURE_2D);
            stbi_image_free(data2);
        }
        else {
            std::cout << "Greska pri ucitavanju teksture!" << std::endl;
        }


        unsigned int basicShader = createShader("basic.vert", "basic.frag");
        unsigned int textShader = createShader("text.vert", "text.frag");
        unsigned int lampShader = createShader("lamp.vert", "lamp.frag");
        unsigned int smokeShader = createShader("smoke.vert", "smoke.frag");
        unsigned int lightShader = createShader("light.vert", "light.frag");


        glm::mat4 projection = glm::ortho(-1.0f, 1.0f, -1.0f, 1.0f);
        glUseProgram(textShader);
        glUniformMatrix4fv(glGetUniformLocation(textShader, "projection"), 1, GL_FALSE, glm::value_ptr(projection));

        glClearColor(0.960784, 0.960784, 0.862745, 1.0);

        while (!glfwWindowShouldClose(window))
        {
            glfwPollEvents();
            glClear(GL_COLOR_BUFFER_BIT);

            if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
                glfwSetWindowShouldClose(window, GL_TRUE);
            }

            if (isBroken) {
                updateBrokenState();
            }
            else {
                updateTimer();
                updateLamp();
            }

            glUseProgram(basicShader);

            glm::mat4 model = glm::mat4(1.0f);
            unsigned int modelLoc = glGetUniformLocation(basicShader, "model");
            glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));

       
            // Postavi uniform vrednost za globalno osvetljenje
            glUniform1f(glGetUniformLocation(basicShader, "globalBrightness"), globalBrightness);

            glUniform1i(glGetUniformLocation(basicShader, "isDoor"), 0);  // Ostali objekti


           //textura
            glActiveTexture(GL_TEXTURE0);                   // Activate texture unit 0
            glBindTexture(GL_TEXTURE_2D, texture);         // Bind the texture
            glUniform1i(glGetUniformLocation(basicShader, "ourTexture"), 0); // Texture unit 0

            glUniform1i(glGetUniformLocation(basicShader, "useTexture"), 0);
            glBindVertexArray(counterTopVAO);
            glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

            // Iscrtavanje prednjeg dela mikrotalasne, boja
            glUniform1i(glGetUniformLocation(basicShader, "useTexture"), 2);
            glBindVertexArray(microwaveVAO);
            glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

            glBindVertexArray(microwaveInsideVAO);
            glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

            glUseProgram(lampShader);
            // Food lamp
            glUniform1i(glGetUniformLocation(lampShader, "isFoodLamp"), 1);  // Ovo je food lamp
           // glUniform3f(glGetUniformLocation(lampShader, "lampColor"), 1.0f, 1.0f, 0.6f); // Žuta boja za food lamp
            glUniform1f(glGetUniformLocation(lampShader, "pulseFactor2"), lampFoodPulse); // Nema pulsiranja za food lamp

            glBindVertexArray(foodLampVAO);
            glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

            glUseProgram(basicShader);

            glActiveTexture(GL_TEXTURE1);                                    // Aktiviraj teksturnu jedinicu 1
            glBindTexture(GL_TEXTURE_2D, foodTexture);                       // druga tekstura
            glUniform1i(glGetUniformLocation(basicShader, "ourTexture2"), 1); // Poveži teksturu sa uniformom

            glUniform1i(glGetUniformLocation(basicShader, "useTexture"), 1); // Izaberi teksturu `food.jpg`
            glBindVertexArray(foodVAO); // VAO za objekat sa teksturom
            glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

            glUniform1i(glGetUniformLocation(basicShader, "useTexture"), 2);

            glBindVertexArray(microwaveSideVAO);
            glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

            glBindVertexArray(microwaveTopVAO);
            glDrawElements(GL_TRIANGLES, 6, 0x1405, 0);

            glBindVertexArray(keyboardFrameVAO);
            glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

            glBindVertexArray(lampVAO);
            glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

            glUseProgram(lampShader); // Aktiviraj shader za lampicu

            glUniform1i(glGetUniformLocation(lampShader, "isFoodLamp"), 0);  


            glUniform3f(glGetUniformLocation(lampShader, "lampColor"), 1.0f, 1.0f, 1.0f); // Crvena boja
            glUniform1f(glGetUniformLocation(lampShader, "pulseFactor"), lampPulse); // Pulsiranje

            glBindVertexArray(lampLineVAO);
            glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

           
            glm::vec3 lightYellow = glm::vec3(1.0f, 1.0f, 0.6f);
            float brightness1 = isRunning ? (0.5f + 0.5f * sin(glfwGetTime() * 2.0f)) : 0.0f;


            glUniform3f(glGetUniformLocation(lampShader, "lampColor"), lightYellow.x, lightYellow.y, lightYellow.z);
            glUniform1f(glGetUniformLocation(lampShader, "brightness"), brightness1);

            glBindVertexArray(foodLampVAO);
            glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
            glBindVertexArray(0);

            glUseProgram(lightShader);

            float currentOpacity = !isRunning ? 0.0f : 0.5f;  // Ako je pokvarena, providnost je 0.0
            // Postavi uniform promenljive za pulsiranje
            glUniform1f(glGetUniformLocation(lightShader, "brightness"), lampPulse);
            glUniform1f(glGetUniformLocation(lightShader, "opacity"), currentOpacity);

            // Iscrtavanje svetlosnog trougla
            glBindVertexArray(foodLampLightVAO);
            glDrawArrays(GL_TRIANGLES, 0, 3);

            glUseProgram(basicShader);  
            glBindVertexArray(timerVAO);
            glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

            glBindVertexArray(keyboardVAO);
            glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

            glBindVertexArray(startStopButtonVAO);
            glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

            glBindVertexArray(restartButtonVAO);
            glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

          
            glm::mat4 doorModel = glm::mat4(1.0f);

            if (isDoorOpen) {
                // Translacija tako da je os rotacije na Y-osi
                glm::mat4 translationToAxis = glm::translate(glm::mat4(1.0f), glm::vec3(0.65f, 0.0f, 0.0f)); // Pomeri prema X=0

                // Rotacija vrata oko Y-ose
                float openAngle = 90.0f; // Ugao otvaranja
                glm::mat4 rotation = glm::rotate(glm::mat4(1.0f), glm::radians(openAngle), glm::vec3(0.0f, 1.0f, 0.0f));

                // Vraćanje nazad na originalnu osu
                glm::mat4 translationBack = glm::translate(glm::mat4(1.0f), glm::vec3(-0.65f, 0.0f, 0.0f));

                // Kombinovana transformacija
                doorModel = translationBack * rotation * translationToAxis;
            }

            //uniform matricu za vrata
            unsigned int doorModelLoc = glGetUniformLocation(basicShader, "doorModel");
            glUniformMatrix4fv(doorModelLoc, 1, GL_FALSE, glm::value_ptr(doorModel));
  
            glUniform1i(glGetUniformLocation(basicShader, "isDoor"), 1); //uniforma za sejder da zna sta crtamo
          
            glUniform1i(glGetUniformLocation(basicShader, "isGlass"), 1); // Poluprovidn
            glUniform1i(glGetUniformLocation(basicShader, "isGlass1"), 0); // Poluprovidno

            glBindVertexArray(doorFrameVAO);
            glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

            // iscrtavanje stakla (poluprovidno)
            glUniform1i(glGetUniformLocation(basicShader, "isGlass"), 1); // Poluprovidno
            glUniform1i(glGetUniformLocation(basicShader, "isGlass1"), 1); // Poluprovidno

            glUniform1f(glGetUniformLocation(basicShader, "transparency"), doorTransparency);
            glBindVertexArray(doorGlassVAO);
            glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

            glUniform1i(glGetUniformLocation(basicShader, "isDoor"), 0);
            glUniform1i(glGetUniformLocation(basicShader, "isGlass"), 0); // Poluprovidno

            glBindVertexArray(pictureVAO);
            glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
            
            // Iscrtavanje teksta tajmera
            glUseProgram(textShader);       
            glBindVertexArray(textVAO);
            if (isBroken) {
              RenderText(textShader, curentTime, -0.07, 0.02, 0.0009, glm::vec3(0.0f, 0.0f, 0.0f), textVAO, textVBO);

            }
            else {
                RenderText(textShader, curentTime, -0.08, 0.02, 0.0015, glm::vec3(0.0f, 0.0f, 0.0f), textVAO, textVBO);
            }

            //iscrtavanje same tastature 
            for (int i = 0; i < 10; ++i) {
                unsigned int buttonVAO = initVAO(numberButtonVertices[i], uniIndices, sizeof(numberButtonVertices[i]), sizeof(uniIndices), 5 * sizeof(float));
                glBindVertexArray(buttonVAO);
                glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
            }

             //Iscrtavanje brojeva tastature
            for (int i = 0; i < 10; ++i) {
                glBindVertexArray(textVAO);
                glEnable(GL_BLEND);
                glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
                char numStr[2];
                snprintf(numStr, sizeof(numStr), "%d", i);
                RenderText(textShader,numStr, numberButtonVertices[i][10] + 0.01f, numberButtonVertices[i][11] + 0.01f, 0.0008, glm::vec3(0.0f, 0.0f, 0.0f), textVAO, textVBO);
            }

            glBindVertexArray(textVAO);
            glEnable(GL_BLEND);
            glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
            RenderText(textShader, "Ana Moraca RA 1/2021", 0.53,0.8, 0.0005, glm::vec3(0.0f, 0.0f, 0.0f), textVAO, textVBO);
            RenderText(textShader, "START  STOP", -0.074, -0.43, 0.00057, glm::vec3(0.0f, 0.0f, 0.0f), textVAO, textVBO);
            RenderText(textShader, "RESTART", -0.062, -0.53, 0.0006, glm::vec3(0.0f, 0.0f, 0.0f), textVAO, textVBO);

            // Ažuriranje uniform promenljivih za dim
            if (isSmokeVisible) {
                glUseProgram(smokeShader);
                glUniform2f(glGetUniformLocation(smokeShader, "center"), smokeCenter.x, smokeCenter.y); // Centar dima
                glUniform2f(glGetUniformLocation(smokeShader, "scale"), smokeScale.x, smokeScale.y);   // Skala elipse
                glUniform1f(glGetUniformLocation(smokeShader, "time"), dimTime);                      // Animacija
                glUniform1f(glGetUniformLocation(smokeShader, "opacity"), maxOpacity);                // Providnost

                glBindVertexArray(smokeVAO);
                glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
            }

            glBindVertexArray(smokeVAO);
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


    int detectButtonClick(double xpos, double ypos) {
        // Koordinate za dugmad od 0 do 9

        std::cout << "Mouse coordinates (normalized): (" << xpos << ", " << ypos << ")" << std::endl;

        struct ButtonBounds {
            float xStart, xEnd, yStart, yEnd;
        };

        ButtonBounds buttons[] = {
            {-0.07f, -0.03f, -0.13f, -0.08f}, // Button 0
            {-0.01f,  0.03f, -0.13f, -0.08f}, // Button 1
            { 0.05f,  0.09f, -0.13f, -0.08f}, // Button 2
            {-0.07f, -0.03f, -0.20f, -0.15f}, // Button 3
            {-0.01f,  0.03f, -0.20f, -0.15f}, // Button 4
            { 0.05f,  0.09f, -0.20f, -0.15f}, // Button 5
            {-0.07f, -0.03f, -0.27f, -0.22f}, // Button 6
            {-0.01f,  0.03f, -0.27f, -0.22f}, // Button 7
            { 0.05f,  0.09f, -0.27f, -0.22f}, // Button 8
            {-0.01f,  0.03f, -0.34f, -0.29f},  // Button 9

            { -0.08f, 0.01f, -0.46f, -0.38f },    // START       
            { 0.01f,  0.1f, -0.46f ,-0.38f },    // RESTART
            { -0.08f, 0.1f ,-0.57f ,- 0.48f, }     // RESTART

        };

        // Iteracija kroz dugmad i provera da li klik odgovara nekom dugmetu
        for (int i = 0; i < 13; ++i) {
            if (xpos >= buttons[i].xStart && xpos <= buttons[i].xEnd &&
                ypos >= buttons[i].yStart && ypos <= buttons[i].yEnd) {
                return i; // Vraća broj dugmeta
            }
        }

        return -1; // Ako klik ne pripada nijednom dugmetu
    }

    void handleKeyPress(int number) {
        std::cout << "Key pressed: " << number << std::endl;

        if (inputCount < 2) {                                   // prva dva klika- unos sekundi

            seconds = seconds * 10 + number;                   // kom desetica i jedinica sekundi
            if ( seconds>59) {
                seconds = seconds - (seconds / 10) * 10;
            }
           
        }
        else if (inputCount < 4) {                           // sledeća dva klika -unos minuta

            minutes = minutes * 10 + number;
            if (minutes > 59) {
                minutes = number;
            }
        }
        else {                                            // više od 4 klika: ignor/  reset

            std::cout << "Maximum input reached. Resetting timer." << std::endl;
            inputCount = -1;                            // Resetuj brojač nakon sledećeg klika
            minutes = 0;
            seconds = 0;
        }

        // ažuriraj trenutni prikaz vremena
        char buffer[6];
        snprintf(buffer, sizeof(buffer), "%02d:%02d", minutes, seconds);
        curentTime = buffer;

        inputCount++;
    }

    void handleButtonPress(int buttonNumber) {
        if (buttonNumber == 10) {                            // Start
            startTimer();
        }
        else if (buttonNumber == 11) {                      // Stop
            stopTimer();
            std::cout << "Microwave stopped." << std::endl;
        }
        else if (buttonNumber == 12) {                      // Restart
            resetTimer();
            std::cout << "Microwave timer reset." << std::endl;
        }
    }

    void updateTimer() {
        static double lastUpdateTime = glfwGetTime();
        if (isRunning) {
            double currentTime = glfwGetTime();
            if (currentTime - lastUpdateTime >= 1.0) {  // Proveri da li je prošla 1 sekunda
                lastUpdateTime = currentTime;

                if (seconds > 0) {
                    seconds--;
                }
                else if (minutes > 0) {
                    minutes--;
                    seconds = 59;
                }
                else {
                    stopTimer(); 
                    isLampOn = true;      
                    isPulsing = false;   

                    std::cout << "Microwave finished!" << std::endl;
                }

                // Ažuriraj prikaz vremena
                char buffer[6];
                snprintf(buffer, sizeof(buffer), "%02d:%02d", minutes, seconds);
                curentTime = buffer;

                updateLamp();
            }
        }
    }

    void resetTimer() {
        isRunning = false;
        minutes = 0;
        seconds = 0;
        curentTime = "00:00";
    }

    void startTimer() {
        if (!isDoorOpen) {
            if (minutes > 0 || seconds > 0) {
                isRunning = true;
                isPulsing = true;           // Lampica počinje da pulsira
                isLampOn = true;          // Lampica nije stalno uključena
            }
        }
        
    }

    void stopTimer() {
        isRunning = false;
        isPulsing = false;          // Prekini pulsiranje
        isLampOn = false;           // Isključi lampicu
    }

    void updateLamp() {
        if (isRunning) {
            double currentTime = glfwGetTime();
            lampPulse = 0.5f + 0.5f * sin(currentTime * 5.14159);  // Sinusni puls (0.0 do 1.0)
            isLampOn = true;  // Lampica svetli
        }
        else {
            lampPulse = 0.0f;  // Lampica ugašena
            isLampOn = false;  // Ne svetli
        }
    }

    void repairMicrowave() {
        isBroken = false;
        isRunning = false;
        isLampOn = false;
        isPulsing = false;
        lampPulse = 1.0f;                                   
        minutes = 0;
        seconds = 0;
        curentTime = "    :";
        sceneBrightness = 1.0f;                                 // reset osvetljenja na početnu vrednost
        smokeCenter = smokeStart;                            
        isSmokeVisible = false;                                 // Dim postaje nevidljiv
        std::cout << "Microwave has been repaired!" << std::endl;
    }

    void updateBrokenState() {
        if (isBroken) {
          
            isSmokeVisible = true;

            if (sceneBrightness > 0.0f) {
                sceneBrightness -= 0.01f;       // Brzina smanjenja
            }
            smokeCenter.y += 0.005f;            // Brzina podizanja dima

            // Kada dim izađe iz gornje granice, resetuj ga na početnu poziciju
            if (smokeCenter.y > 1.2f) {
                smokeCenter = smokeStart; // Vraćanje na početnu poziciju
            }

            dimTime += 0.01f;                   // Animacija u vremenu

            double currentTime = glfwGetTime();
            if (currentTime - lastBlinkTime > 0.5) {
                isTextBlinking = !isTextBlinking;
                lastBlinkTime = currentTime;
            }

            curentTime = isTextBlinking ? "ERROR" : "";
        }
        else {
            smokeCenter = smokeStart; // Reset dima
        }
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

    void loadFont(const char* fontPath) {
        FT_Library ft;
        if (FT_Init_FreeType(&ft)) {
            std::cout << "ERROR::FREETYPE: Could not init FreeType Library" << std::endl;
            return;
        }

        FT_Face face;                                   // velicina karaktera ..
        if (FT_New_Face(ft, fontPath, 0, &face)) {
            std::cout << "ERROR::FREETYPE: Failed to load font" << std::endl;
            return;
        }

        FT_Set_Pixel_Sizes(face, 0, 48);
        glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

        // Učitavanje svih ASCII karaktera
        for (unsigned char c = 0; c < 128; c++) {
            if (FT_Load_Char(face, c, FT_LOAD_RENDER)) {
                std::cout << "ERROR::FREETYPE: Failed to load Glyph" << std::endl;
                continue;
            }

            unsigned int texture;
            glGenTextures(1, &texture);
            glBindTexture(GL_TEXTURE_2D, texture);
            glTexImage2D(
                GL_TEXTURE_2D, 0, GL_RED,
                face->glyph->bitmap.width, face->glyph->bitmap.rows,
                0, GL_RED, GL_UNSIGNED_BYTE, face->glyph->bitmap.buffer
            );

            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

            Character character = {                                    // Skladišti informacije o teksturi, dimenzijama, pozicioniranju i razmaku.
                texture,
                glm::ivec2(face->glyph->bitmap.width, face->glyph->bitmap.rows),
                glm::ivec2(face->glyph->bitmap_left, face->glyph->bitmap_top),
                face->glyph->advance.x
            };
            Characters.insert(std::pair<char, Character>(c, character));
        }

        FT_Done_Face(face);
        FT_Done_FreeType(ft);
    }

    void RenderText(unsigned int shader, const std::string& text, float x, float y, float scale, glm::vec3 color, unsigned int& VAO, unsigned int& VBO) {
        glUseProgram(shader);

        glUniform3f(glGetUniformLocation(shader, "textColor"), color.x, color.y, color.z);
        glActiveTexture(GL_TEXTURE0);
        glBindVertexArray(VAO);

        for (char c : text) {
            Character ch = Characters[c];

            float xpos = x + ch.Bearing.x * scale;          
            float ypos = y - (ch.Size.y - ch.Bearing.y) * scale;

            float w = ch.Size.x * scale;
            float h = ch.Size.y * scale;

            float vertices[6][4] = {
                { xpos,     ypos + h,   0.0f, 0.0f },
                { xpos,     ypos,       0.0f, 1.0f },
                { xpos + w, ypos,       1.0f, 1.0f },

                { xpos,     ypos + h,   0.0f, 0.0f },
                { xpos + w, ypos,       1.0f, 1.0f },
                { xpos + w, ypos + h,   1.0f, 0.0f }
            };

            glBindTexture(GL_TEXTURE_2D, ch.TextureID);
            glBindBuffer(GL_ARRAY_BUFFER, VBO);
            glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);
            glDrawArrays(GL_TRIANGLES, 0, 6);

            x += (ch.Advance >> 6) * scale;     //pomera kursor za sirinu trenurnog
        }

        glBindVertexArray(0);
        glBindTexture(GL_TEXTURE_2D, 0);
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


