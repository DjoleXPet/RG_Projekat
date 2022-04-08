#include <iostream>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <Shader.h>
#include <Camera.h>
#include <stb_image.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <Model.h>
#include <Lampa.h>

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"


void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow *window);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void key_callback(GLFWwindow *window, int key, int scancode, int action, int mods);

void renderQuad();
void DrawImGui();

unsigned int loadTexture(char const * path);
unsigned int loadCubemap(vector<std::string> faces);


const unsigned int SCR_WIDTH = 1280;
const unsigned int SCR_HEIGHT = 720;

Camera camera = Camera();
Lampa lampaSettings = Lampa();

bool firstMouse = true;
float lastX =  SCR_WIDTH / 2.0;
float lastY =  SCR_HEIGHT / 2.0;

bool bloom = true;
bool bloomKeyPressed = false;
float exposure = 1.0f;

bool ImGuiEnabled = false;

float deltaTime = 0.0f;	// Time between current frame and last frame
float lastFrame = 0.0f; // Time of last frame

glm::vec3 lightPos = glm::vec3(20,20,20);



int main() {

    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 5);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "RG_Projekat", nullptr, nullptr);
    if(window == nullptr)
    {
        std::cout << "Failed to create window";
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    if (ImGuiEnabled) {
        glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
    }
    // Init Imgui
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO &io = ImGui::GetIO();
    (void) io;

    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 450 core");

    glEnable(GL_DEPTH_TEST);
    // SHADER
    Shader shaderGround("../resources/shaders/ground.vs", "../resources/shaders/ground.fs");
    Shader skyboxShader("../resources/shaders/skybox.vs","../resources/shaders/skybox.fs");
    Shader shaderModel("../resources/shaders/model.vs", "../resources/shaders/model.fs");
    Shader shaderBlend("../resources/shaders/blend.vs", "../resources/shaders/blend.fs");
    Shader shaderLamp("../resources/shaders/lampa.vs", "../resources/shaders/lampa.fs");
    Shader shaderMoon("../resources/shaders/moon.vs", "../resources/shaders/moon.fs");

    float groundVertices[] = {
                // koordinate        // teksture
            5, -0.5, 5, 0,0,
            5,-0.5,-5, 50,0,
            -5,-0.5,5, 0,50,

            -5,-0.5,5, 0,50,
            5,-0.5,-5, 50,0,
            -5, -0.5, -5, 50,50
    };

    float skyboxVertices[] = {
            // positions
            -1.0f,  1.0f, -1.0f,
            -1.0f, -1.0f, -1.0f,
            1.0f, -1.0f, -1.0f,
            1.0f, -1.0f, -1.0f,
            1.0f,  1.0f, -1.0f,
            -1.0f,  1.0f, -1.0f,

            -1.0f, -1.0f,  1.0f,
            -1.0f, -1.0f, -1.0f,
            -1.0f,  1.0f, -1.0f,
            -1.0f,  1.0f, -1.0f,
            -1.0f,  1.0f,  1.0f,
            -1.0f, -1.0f,  1.0f,

            1.0f, -1.0f, -1.0f,
            1.0f, -1.0f,  1.0f,
            1.0f,  1.0f,  1.0f,
            1.0f,  1.0f,  1.0f,
            1.0f,  1.0f, -1.0f,
            1.0f, -1.0f, -1.0f,

            -1.0f, -1.0f,  1.0f,
            -1.0f,  1.0f,  1.0f,
            1.0f,  1.0f,  1.0f,
            1.0f,  1.0f,  1.0f,
            1.0f, -1.0f,  1.0f,
            -1.0f, -1.0f,  1.0f,

            -1.0f,  1.0f, -1.0f,
            1.0f,  1.0f, -1.0f,
            1.0f,  1.0f,  1.0f,
            1.0f,  1.0f,  1.0f,
            -1.0f,  1.0f,  1.0f,
            -1.0f,  1.0f, -1.0f,

            -1.0f, -1.0f, -1.0f,
            -1.0f, -1.0f,  1.0f,
            1.0f, -1.0f, -1.0f,
            1.0f, -1.0f, -1.0f,
            -1.0f, -1.0f,  1.0f,
            1.0f, -1.0f,  1.0f
    };

    unsigned int groundVAO, groundVBO;
    glGenVertexArrays(1, &groundVAO);
    glGenBuffers(1, &groundVBO);
    glBindVertexArray(groundVAO);
    glBindBuffer(GL_ARRAY_BUFFER, groundVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(groundVertices), &groundVertices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3*sizeof(float)));
    glEnableVertexAttribArray(1);

    unsigned int groundDiffText = loadTexture("../resources/textures/trava/seamless.jpg");   // NOVA TEKSTURA
    shaderGround.use();
    shaderGround.setInt("texture1", 0);

  //  stbi_set_flip_vertically_on_load(true);

    // skybox VAO
    unsigned int skyboxVAO, skyboxVBO;
    glGenVertexArrays(1, &skyboxVAO);
    glGenBuffers(1, &skyboxVBO);
    glBindVertexArray(skyboxVAO);
    glBindBuffer(GL_ARRAY_BUFFER, skyboxVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(skyboxVertices), &skyboxVertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);

    vector<std::string> faces
            {
                    "../resources/textures/skybox/Standard-Cube-Map/px.png", //desno
                    "../resources/textures/skybox/Standard-Cube-Map/nx.png", //levo
                    "../resources/textures/skybox/Standard-Cube-Map/py.png", //gore
                    "../resources/textures/skybox/Standard-Cube-Map/ny.png", //dole
                    "../resources/textures/skybox/Standard-Cube-Map/pz.png", //napred
                    "../resources/textures/skybox/Standard-Cube-Map/nz.png" //nazad
            };
    unsigned int cubemapTexture = loadCubemap(faces);
    skyboxShader.use();
    skyboxShader.setInt("skybox", 0);



    // vegetacija

    float transparentVertices[] = {
            // positions         // texture Coords            // normals
            -5.0f,  0.5f,  -5.0f,  0.0f,  0.0f, 0,0,1,
            -5.0f, -0.5f,  -5.0f,  0.0f,  1.0f,0,0,1,
            5.0f, -0.5f,  -5.0f,  3.0f,  1.0f,0,0,1,

            -5.0f,  0.5f,  -5.0f,  0.0f,  0.0f,0,0,1,
            5.0f, -0.5f,  -5.0f,  3.0f,  1.0f,0,0,1,
            5.0f,  0.5f,  -5.0f,  3.0f,  0.0f,0,0,1
    };
    unsigned int transparentVAO, transparentVBO;
    glGenVertexArrays(1, &transparentVAO);
    glGenBuffers(1, &transparentVBO);
    glBindVertexArray(transparentVAO);
    glBindBuffer(GL_ARRAY_BUFFER, transparentVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(transparentVertices), transparentVertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(5 * sizeof(float)));

    glBindVertexArray(0);
    unsigned int transparentTexture = loadTexture("../resources/textures/zbun2crop.png");
    shaderBlend.use();
    shaderBlend.setInt("texture1", 0);


    glm::vec3 treePositions[] = {  // POCETNE KOORDINATE DRVECA SU ~ 2, 0, 2
            glm::vec3( 4.5f,  -0.7f,  4.5f),
            glm::vec3( -4.5f,  -0.7f, -4.5f),
            glm::vec3(2, -0.7f, -3),
            glm::vec3(-3.8f, -0.7f, 2.3f),
            glm::vec3( 4.0f, -0.7f, -3.5f),
            glm::vec3(0,  -0.7f, -2.0f),
            glm::vec3( 3.3f, -0.7f, -2.5f),
            glm::vec3( -4.5f,  -0.7f, 4.5f),
            glm::vec3( -2.5f,  -0.7f, 0.5f),
            glm::vec3(0,  -0.7f, -4.5f),
            glm::vec3(-3.0f,  -0.7f, 0.0f),
            glm::vec3( -2.3f, -0.7f, -2.5f),
            glm::vec3( 3.5f,  -0.7f, 0.0f),
            glm::vec3( -1.5f,  -0.7f, 4.0f),
            glm::vec3(2.0f,  -0.7f, 3.5f)
    };


    // HDR BLOOM
    Shader shaderBlur("../resources/shaders/blur.vs", "../resources/shaders/blur.fs");
    Shader shaderBloom("../resources/shaders/bloom.vs", "../resources/shaders/bloom.fs");

    shaderBlur.use();
    shaderBlur.setInt("image", 0);



    shaderBloom.use();
    shaderBloom.setInt("scene", 0);
    shaderBloom.setInt("bloomBlur", 1);


    unsigned int hdrFBO;
    glGenFramebuffers(1, &hdrFBO);
    glBindFramebuffer(GL_FRAMEBUFFER, hdrFBO);
    // create 2 floating point color buffers (1 for normal rendering, other for brightness threshold values)
    unsigned int colorBuffers[2];
    glGenTextures(2, colorBuffers);
    for (unsigned int i = 0; i < 2; i++)
    {
        glBindTexture(GL_TEXTURE_2D, colorBuffers[i]);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, SCR_WIDTH, SCR_HEIGHT, 0, GL_RGBA, GL_FLOAT, NULL);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);  // we clamp to the edge as the blur filter would otherwise sample repeated texture values!
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        // attach texture to framebuffer
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + i, GL_TEXTURE_2D, colorBuffers[i], 0);
    }
    // create and attach depth buffer (renderbuffer)
    unsigned int rboDepth;
    glGenRenderbuffers(1, &rboDepth);
    glBindRenderbuffer(GL_RENDERBUFFER, rboDepth);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, SCR_WIDTH, SCR_HEIGHT);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, rboDepth);
    // tell OpenGL which color attachments we'll use (of this framebuffer) for rendering
    unsigned int attachments[2] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1 };
    glDrawBuffers(2, attachments);
    // finally check if framebuffer is complete
    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        std::cout << "Framebuffer not complete!" << std::endl;
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    // ping-pong-framebuffer for blurring
    unsigned int pingpongFBO[2];
    unsigned int pingpongColorbuffers[2];
    glGenFramebuffers(2, pingpongFBO);
    glGenTextures(2, pingpongColorbuffers);
    for (unsigned int i = 0; i < 2; i++)
    {
        glBindFramebuffer(GL_FRAMEBUFFER, pingpongFBO[i]);
        glBindTexture(GL_TEXTURE_2D, pingpongColorbuffers[i]);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, SCR_WIDTH, SCR_HEIGHT, 0, GL_RGBA, GL_FLOAT, NULL);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE); // we clamp to the edge as the blur filter would otherwise sample repeated texture values!
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, pingpongColorbuffers[i], 0);
        // also check if framebuffers are complete (no need for depth buffer)
        if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
            std::cout << "Framebuffer not complete!" << std::endl;
    }

    // / //////////////////////
    Model sator("../resources/objects/sator3/10495_Green_Mesh_Tent_V1_L3.obj");
    Model drvo("../resources/objects/drvo/drvo.obj");
    Model lampa("../resources/objects/lampa/Flashlight.obj");
    Model mesec("../resources/objects/mesec/Moon_2K.obj");
    Model patka("../resources/objects/patka/12248_Bird_v1_L2.obj");


    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetScrollCallback(window, scroll_callback);
    glfwSetKeyCallback(window, key_callback);
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);


    camera.Position = glm::vec3(0,0,3);
    camera.Front = glm::vec3(0,0,-1);
    camera.WorldUp = glm::vec3(0,1,0);


    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);

    lampaSettings.position = glm::vec3(2, 0.5, 2);
    lampaSettings.direction = glm::vec3(0,0,-1);

    lampaSettings.ambient = glm::vec3( 0.0f, 0.0f, 0.0f);
    lampaSettings.diffuse = glm::vec3(1.0f,1.0f, 1.0f);
    lampaSettings.specular = glm::vec3(1.0f, 1.0f, 0.0f);
    lampaSettings.constant = 1.0f;
    lampaSettings.linear = 0.09f;
    lampaSettings.quadratic = 0.032f;
    lampaSettings.cutOff = glm::cos(glm::radians(20.0f));
    lampaSettings.outerCutOff =  glm::cos(glm::radians(35.0f));



    while (!glfwWindowShouldClose(window))
    {
        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        processInput(window);

        //  HDR
        glBindFramebuffer(GL_FRAMEBUFFER, hdrFBO);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


        //    lightPos = glm::vec3(cos(currentFrame/5)*25, sin(currentFrame/5)*25,25);

        lampaSettings.direction = lampaSettings.Front;

        // BOJA SVETLA MESEC
        glm::vec3 lightColor = glm::vec3(0.0096f, 0.0093f,0.0084f);   // smanji za 10 za mrak


        glm::mat4 view          = glm::mat4(1.0f); // make sure to initialize matrix to identity matrix first
        glm::mat4 projection    = glm::mat4(1.0f);
        glm::mat4 model = glm::mat4(1.0f);
        projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
        view = camera.GetViewMatrix();



        // mesec
        shaderMoon.use();
        shaderMoon.setUniformMat4("projection", projection);
        shaderMoon.setUniformMat4("view", view);

        shaderMoon.setUniform3f("dirLight.direction", -lightPos);  // mesec se obasjava sa druge strane
        shaderMoon.setUniform3f("dirLight.ambient", lightColor);
        shaderMoon.setUniform3f("dirLight.diffuse", 5.0f*lightColor);
        shaderMoon.setUniform3f("dirLight.specular", lightColor);

        shaderMoon.setUniform3f("viewPos", camera.Position);

        model = glm::mat4(1.0f);
        model = glm::translate(model, lightPos);
        //  model = glm::scale(model, glm::vec3(0.1,0.1,0.1));
        shaderMoon.setUniformMat4("model", model);
        mesec.Draw(shaderMoon);


        // ground
        shaderGround.use();

        model = glm::mat4(1.0f);

        shaderGround.setUniformMat4("projection", projection);
        shaderGround.setUniformMat4("view", view);
        shaderGround.setUniformMat4("model", model);

        shaderGround.setUniform3f("dirLight.direction", lightPos);
        shaderGround.setUniform3f("dirLight.ambient", lightColor);
        shaderGround.setUniform3f("dirLight.diffuse", lightColor);
        shaderGround.setUniform3f("dirLight.specular",  lightColor);

        shaderGround.setUniform3f("spotLight.position",  lampaSettings.position);
        shaderGround.setUniform3f("spotLight.direction", lampaSettings.direction);
        shaderGround.setUniform3f("spotLight.ambient", lampaSettings.ambient);
        shaderGround.setUniform3f("spotLight.diffuse", lampaSettings.diffuse);
        shaderGround.setUniform3f("spotLight.specular", lampaSettings.specular);
        shaderGround.setFloat("spotLight.constant", lampaSettings.constant);
        shaderGround.setFloat("spotLight.linear", lampaSettings.linear);
        shaderGround.setFloat("spotLight.quadratic", lampaSettings.quadratic);
        shaderGround.setFloat("spotLight.cutOff", lampaSettings.cutOff);
        shaderGround.setFloat("spotLight.outerCutOff", lampaSettings.outerCutOff);

        shaderGround.setUniform3f("viewPos", camera.Position);

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, groundDiffText);

        glBindVertexArray(groundVAO);
        glDrawArrays(GL_TRIANGLES, 0, 6);


        shaderModel.use();
        shaderModel.setUniformMat4("projection", projection);
        shaderModel.setUniformMat4("view", view);

        shaderModel.setUniform3f("dirLight.direction", -lightPos);
        shaderModel.setUniform3f("dirLight.ambient", lightColor); //
        shaderModel.setUniform3f("dirLight.diffuse", lightColor);
        shaderModel.setUniform3f("dirLight.specular", lightColor);

        shaderModel.setUniform3f("spotLight.position", lampaSettings.position);
        shaderModel.setUniform3f("spotLight.direction", lampaSettings.direction);
        shaderModel.setUniform3f("spotLight.ambient", lampaSettings.ambient);
        shaderModel.setUniform3f("spotLight.diffuse", lampaSettings.diffuse);
        shaderModel.setUniform3f("spotLight.specular", lampaSettings.specular);
        shaderModel.setFloat("spotLight.constant", lampaSettings.constant);
        shaderModel.setFloat("spotLight.linear", lampaSettings.linear);
        shaderModel.setFloat("spotLight.quadratic", lampaSettings.quadratic);
        shaderModel.setFloat("spotLight.cutOff", lampaSettings.cutOff);
        shaderModel.setFloat("spotLight.outerCutOff", lampaSettings.outerCutOff);

        shaderModel.setUniform3f("viewPos", camera.Position);




        // sator
        model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(0,-0.5f,0));
        model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(1,0,0));
        model = glm::scale(model, glm::vec3(0.01,0.01,0.01));

        shaderModel.setUniformMat4("model", model);

        sator.Draw(shaderModel);

        // patka

        model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(2.5f,-0.5f,-4));
        model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(1,0,0));
        model = glm::scale(model, glm::vec3(0.01,0.01,0.01));

        shaderModel.setUniformMat4("model", model);

        patka.Draw(shaderModel);


        for (unsigned int i = 0; i < 15; i++)
        {
            model = glm::mat4(1.0f);
            model = glm::translate(model, glm::vec3(2, 0, 2));  // pomera se drvo u 0,0,0
            model = glm::translate(model, treePositions[i]);
            model = glm::rotate(model, glm::radians(-90.f), glm::vec3(1,0,0));
            model = glm::scale(model, glm::vec3(0.01,0.01,0.01));

            shaderModel.setUniformMat4("model", model);
            drvo.Draw(shaderModel);
        }
        // lampa
        shaderLamp.use();
        shaderLamp.setUniformMat4("projection", projection);
        shaderLamp.setUniformMat4("view", view);

        shaderLamp.setUniform3f("dirLight.direction", lightPos);
        shaderLamp.setUniform3f("dirLight.ambient", lightColor); //
        shaderLamp.setUniform3f("dirLight.diffuse", lightColor);
        shaderLamp.setUniform3f("dirLight.specular", lightColor);

        shaderLamp.setUniform3f("spotLight.position", lampaSettings.position);
        shaderLamp.setUniform3f("spotLight.direction", lampaSettings.direction);
        shaderLamp.setUniform3f("spotLight.ambient", lampaSettings.diffuse); // da izgleda kao ista boja
        shaderLamp.setUniform3f("spotLight.diffuse", lampaSettings.diffuse);
        shaderLamp.setUniform3f("spotLight.specular", lampaSettings.specular);
        shaderLamp.setFloat("spotLight.constant", lampaSettings.constant);
        shaderLamp.setFloat("spotLight.linear", lampaSettings.linear);
        shaderLamp.setFloat("spotLight.quadratic", lampaSettings.quadratic);
        shaderLamp.setFloat("spotLight.cutOff", lampaSettings.cutOff);
        shaderLamp.setFloat("spotLight.outerCutOff", lampaSettings.outerCutOff);

        shaderLamp.setUniform3f("viewPos", camera.Position);

        model = glm::mat4(1.0f);
        model = glm::translate(model, lampaSettings.position);

        model = glm::rotate(model, glm::radians(-lampaSettings.Yaw), lampaSettings.Up);
        model = glm::rotate(model, glm::radians(lampaSettings.Pitch), lampaSettings.Right);
        model = glm::rotate(model, glm::radians(90.0f), lampaSettings.WorldUp);


        model = glm::scale(model, glm::vec3(0.01,0.01,0.01));
        shaderLamp.setUniformMat4("model", model);

        lampa.Draw(shaderLamp);


        // vegetacija
        shaderBlend.use();
        glBindVertexArray(transparentVAO);
        glBindTexture(GL_TEXTURE_2D, transparentTexture);
        shaderBlend.setUniformMat4("view", view);
        shaderBlend.setUniformMat4("projection", projection);

        shaderBlend.setUniform3f("dirLight.direction", lightPos);
        shaderBlend.setUniform3f("dirLight.ambient", lightColor);
        shaderBlend.setUniform3f("dirLight.diffuse", lightColor);
        shaderBlend.setUniform3f("dirLight.specular", lightColor);

        shaderBlend.setUniform3f("spotLight.position",  lampaSettings.position);
        shaderBlend.setUniform3f("spotLight.direction", lampaSettings.direction);
        shaderBlend.setUniform3f("spotLight.ambient", lampaSettings.ambient);
        shaderBlend.setUniform3f("spotLight.diffuse", lampaSettings.diffuse);
        shaderBlend.setUniform3f("spotLight.specular", lampaSettings.specular);
        shaderBlend.setFloat("spotLight.constant", lampaSettings.constant);
        shaderBlend.setFloat("spotLight.linear", lampaSettings.linear);
        shaderBlend.setFloat("spotLight.quadratic", lampaSettings.quadratic);
        shaderBlend.setFloat("spotLight.cutOff", lampaSettings.cutOff);
        shaderBlend.setFloat("spotLight.outerCutOff", lampaSettings.outerCutOff);

        shaderBlend.setUniform3f("viewPos", camera.Position);

        model = glm::mat4(1.0f);

        for(int i = 0; i < 4; i++){
            model = glm::rotate(model, glm::radians(i*90.0f), glm::vec3(0,1,0));
            shaderBlend.setUniformMat4("model", model);
            glDrawArrays(GL_TRIANGLES, 0, 6);
        }




        glDepthFunc(GL_LEQUAL);  // change depth function so depth test passes when values are equal to depth buffer's content
        skyboxShader.use();
        view = glm::mat4(glm::mat3(camera.GetViewMatrix())); // remove translation from the view matrix
        skyboxShader.setUniformMat4("view", view);
        skyboxShader.setUniformMat4("projection", projection);

        // skybox cube
        glBindVertexArray(skyboxVAO);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_CUBE_MAP, cubemapTexture);
        glDrawArrays(GL_TRIANGLES, 0, 36);
        glBindVertexArray(0);
        glDepthFunc(GL_LESS); // set depth function back to default

        // HDR BLOOM
        glBindFramebuffer(GL_FRAMEBUFFER, 0);

        // 2. blur bright fragments with two-pass Gaussian Blur
        // --------------------------------------------------
        bool horizontal = true, first_iteration = true;
        unsigned int amount = 10;
        shaderBlur.use();
        for (unsigned int i = 0; i < amount; i++)
        {
            glBindFramebuffer(GL_FRAMEBUFFER, pingpongFBO[horizontal]);
            shaderBlur.setInt("horizontal", horizontal);
            glBindTexture(GL_TEXTURE_2D, first_iteration ? colorBuffers[1] : pingpongColorbuffers[!horizontal]);  // bind texture of other framebuffer (or scene if first iteration)
            renderQuad();
            horizontal = !horizontal;
            if (first_iteration)
                first_iteration = false;
        }
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        shaderBloom.use();

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, colorBuffers[0]);
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, pingpongColorbuffers[!horizontal]);

        shaderBloom.setInt("bloom", bloom);
        shaderBloom.setFloat("exposure", exposure);
        renderQuad();

     //   std::cout << "bloom: " << (bloom ? "on" : "off") << std::endl;


        if (ImGuiEnabled)
            DrawImGui();

        glfwSwapBuffers(window);
        glfwPollEvents();
    }



    shaderGround.deleteProgram();
    shaderModel.deleteProgram();
    shaderBlend.deleteProgram();
    shaderLamp.deleteProgram();
    shaderMoon.deleteProgram();
    shaderBloom.deleteProgram();
    shaderBlur.deleteProgram();


    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    glfwTerminate();
    return 0;
}

unsigned int quadVAO = 0;
unsigned int quadVBO;

void renderQuad()
{
    if (quadVAO == 0)
    {
        float quadVertices[] = {
                // positions        // texture Coords
                -1.0f,  1.0f, 0.0f, 0.0f, 1.0f,
                -1.0f, -1.0f, 0.0f, 0.0f, 0.0f,
                1.0f,  1.0f, 0.0f, 1.0f, 1.0f,
                1.0f, -1.0f, 0.0f, 1.0f, 0.0f,
        };
        // setup plane VAO
        glGenVertexArrays(1, &quadVAO);
        glGenBuffers(1, &quadVBO);
        glBindVertexArray(quadVAO);
        glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
    }
    glBindVertexArray(quadVAO);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    glBindVertexArray(0);
}
void processInput(GLFWwindow *window) {
    if(glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        camera.ProcessKeyboard(FORWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        camera.ProcessKeyboard(BACKWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        camera.ProcessKeyboard(LEFT, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        camera.ProcessKeyboard(RIGHT, deltaTime);

    // lampa

    if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS)
        lampaSettings.ProcessKeyboard(GORE, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS)
        lampaSettings.ProcessKeyboard(DOLE, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS)
        lampaSettings.ProcessKeyboard(LEVO, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS)
        lampaSettings.ProcessKeyboard(DESNO, deltaTime);

    // bloom

    if (glfwGetKey(window, GLFW_KEY_B) == GLFW_PRESS && !bloomKeyPressed)
    {
        bloom = !bloom;
        bloomKeyPressed = true;
    }
    if (glfwGetKey(window, GLFW_KEY_B) == GLFW_RELEASE)
    {
        bloomKeyPressed = false;
    }



}

void key_callback(GLFWwindow *window, int key, int scancode, int action, int mods) {
    if (key == GLFW_KEY_TAB && action == GLFW_PRESS) {
        ImGuiEnabled = !ImGuiEnabled;
        if (ImGuiEnabled) {
            // CameraMouseMovementUpdateEnabled = false;
            glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
        } else {
            glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
        }
    }
}



void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    // make sure the viewport matches the new window dimensions; note that width and
    // height will be significantly larger than specified on retina displays.
    glViewport(0, 0, width, height);
}

void mouse_callback(GLFWwindow* window, double xposIn, double yposIn)
{
    float xpos = static_cast<float>(xposIn);
    float ypos = static_cast<float>(yposIn);

    if (firstMouse)
    {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }

    float xoffset = xpos - lastX;
    float yoffset = lastY - ypos; // reversed since y-coordinates go from bottom to top

    lastX = xpos;
    lastY = ypos;
    if(ImGuiEnabled == false)
        camera.ProcessMouseMovement(xoffset, yoffset);
}

// glfw: whenever the mouse scroll wheel scrolls, this callback is called
// ----------------------------------------------------------------------
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
    camera.ProcessMouseScroll(yoffset);
}

unsigned int loadTexture(char const * path)
{
    unsigned int textureID;
    glGenTextures(1, &textureID);

    int width, height, nrComponents;
    unsigned char *data = stbi_load(path, &width, &height, &nrComponents, 0);
    if (data)
    {
        GLenum format;
        if (nrComponents == 1)
            format = GL_RED;
        else if (nrComponents == 3)
            format = GL_RGB;
        else if (nrComponents == 4)
            format = GL_RGBA;

        glBindTexture(GL_TEXTURE_2D, textureID);
        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT); //format == GL_RGBA ? GL_CLAMP_TO_EDGE : GL_REPEAT); // for this tutorial: use GL_CLAMP_TO_EDGE to prevent semi-transparent borders. Due to interpolation it takes texels from next repeat
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, format == GL_RGBA ? GL_CLAMP_TO_EDGE : GL_REPEAT);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        stbi_image_free(data);
    }
    else
    {
        std::cout << "Texture failed to load at path: " << path << std::endl;
        stbi_image_free(data);
    }

    return textureID;
}

unsigned int loadCubemap(vector<std::string> faces)
{
    unsigned int textureID;
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);

    int width, height, nrChannels;
    for (unsigned int i = 0; i < faces.size(); i++)
    {
        unsigned char *data = stbi_load(faces[i].c_str(), &width, &height, &nrChannels, 0);
        GLenum format;
        if (nrChannels == 1)
            format = GL_RED;
        else if (nrChannels == 3)
            format = GL_RGB;
        else if (nrChannels == 4)
            format = GL_RGBA;
        if (data)
        {
            glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
            stbi_image_free(data);

        }
        else
        {
            std::cout << "Cubemap texture failed to load at path: " << faces[i] << std::endl;
            stbi_image_free(data);
        }
    }
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

    return textureID;
}

// renderCube() renders a 1x1 3D cube in NDC.
// -------------------------------------------------
unsigned int cubeVAO = 0;
unsigned int cubeVBO = 0;
void renderCube()
{
    // initialize (if necessary)
    if (cubeVAO == 0)
    {
        float vertices[] = {
                // back face
                -1.0f, -1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 0.0f, 0.0f, // bottom-left
                1.0f,  1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 1.0f, 1.0f, // top-right
                1.0f, -1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 1.0f, 0.0f, // bottom-right
                1.0f,  1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 1.0f, 1.0f, // top-right
                -1.0f, -1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 0.0f, 0.0f, // bottom-left
                -1.0f,  1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 0.0f, 1.0f, // top-left
                // front face
                -1.0f, -1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f, 0.0f, // bottom-left
                1.0f, -1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 1.0f, 0.0f, // bottom-right
                1.0f,  1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 1.0f, 1.0f, // top-right
                1.0f,  1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 1.0f, 1.0f, // top-right
                -1.0f,  1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f, 1.0f, // top-left
                -1.0f, -1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f, 0.0f, // bottom-left
                // left face
                -1.0f,  1.0f,  1.0f, -1.0f,  0.0f,  0.0f, 1.0f, 0.0f, // top-right
                -1.0f,  1.0f, -1.0f, -1.0f,  0.0f,  0.0f, 1.0f, 1.0f, // top-left
                -1.0f, -1.0f, -1.0f, -1.0f,  0.0f,  0.0f, 0.0f, 1.0f, // bottom-left
                -1.0f, -1.0f, -1.0f, -1.0f,  0.0f,  0.0f, 0.0f, 1.0f, // bottom-left
                -1.0f, -1.0f,  1.0f, -1.0f,  0.0f,  0.0f, 0.0f, 0.0f, // bottom-right
                -1.0f,  1.0f,  1.0f, -1.0f,  0.0f,  0.0f, 1.0f, 0.0f, // top-right
                // right face
                1.0f,  1.0f,  1.0f,  1.0f,  0.0f,  0.0f, 1.0f, 0.0f, // top-left
                1.0f, -1.0f, -1.0f,  1.0f,  0.0f,  0.0f, 0.0f, 1.0f, // bottom-right
                1.0f,  1.0f, -1.0f,  1.0f,  0.0f,  0.0f, 1.0f, 1.0f, // top-right
                1.0f, -1.0f, -1.0f,  1.0f,  0.0f,  0.0f, 0.0f, 1.0f, // bottom-right
                1.0f,  1.0f,  1.0f,  1.0f,  0.0f,  0.0f, 1.0f, 0.0f, // top-left
                1.0f, -1.0f,  1.0f,  1.0f,  0.0f,  0.0f, 0.0f, 0.0f, // bottom-left
                // bottom face
                -1.0f, -1.0f, -1.0f,  0.0f, -1.0f,  0.0f, 0.0f, 1.0f, // top-right
                1.0f, -1.0f, -1.0f,  0.0f, -1.0f,  0.0f, 1.0f, 1.0f, // top-left
                1.0f, -1.0f,  1.0f,  0.0f, -1.0f,  0.0f, 1.0f, 0.0f, // bottom-left
                1.0f, -1.0f,  1.0f,  0.0f, -1.0f,  0.0f, 1.0f, 0.0f, // bottom-left
                -1.0f, -1.0f,  1.0f,  0.0f, -1.0f,  0.0f, 0.0f, 0.0f, // bottom-right
                -1.0f, -1.0f, -1.0f,  0.0f, -1.0f,  0.0f, 0.0f, 1.0f, // top-right
                // top face
                -1.0f,  1.0f, -1.0f,  0.0f,  1.0f,  0.0f, 0.0f, 1.0f, // top-left
                1.0f,  1.0f , 1.0f,  0.0f,  1.0f,  0.0f, 1.0f, 0.0f, // bottom-right
                1.0f,  1.0f, -1.0f,  0.0f,  1.0f,  0.0f, 1.0f, 1.0f, // top-right
                1.0f,  1.0f,  1.0f,  0.0f,  1.0f,  0.0f, 1.0f, 0.0f, // bottom-right
                -1.0f,  1.0f, -1.0f,  0.0f,  1.0f,  0.0f, 0.0f, 1.0f, // top-left
                -1.0f,  1.0f,  1.0f,  0.0f,  1.0f,  0.0f, 0.0f, 0.0f  // bottom-left
        };
        glGenVertexArrays(1, &cubeVAO);
        glGenBuffers(1, &cubeVBO);
        // fill buffer
        glBindBuffer(GL_ARRAY_BUFFER, cubeVBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
        // link vertex attributes
        glBindVertexArray(cubeVAO);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
        glEnableVertexAttribArray(2);
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindVertexArray(0);
    }
    // render Cube
    glBindVertexArray(cubeVAO);
    glDrawArrays(GL_TRIANGLES, 0, 36);
    glBindVertexArray(0);
}


void DrawImGui() {
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();


    {
        ImGui::Begin("Lamp Settings");
        ImGui::ColorEdit3("Lamp Diffuse color" , (float*)&lampaSettings.diffuse);
        ImGui::ColorEdit3("Lamp Specular color" , (float*)&lampaSettings.specular);
        ImGui::DragFloat("Lamp outer cutoff", (float*)&lampaSettings.outerCutOff, 0.1f,glm::radians(20.0f),glm::radians(50.0f));

        ImGui::End();
    }

    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}
