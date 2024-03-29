#define STB_IMAGE_IMPLEMENTATION
#include <iostream>
#include <filesystem>

#include "stb_image.h"
#include <glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "camera.h"
#include "shader.h"


// settings
//const unsigned int SCR_WIDTH = 1280;
//const unsigned int SCR_HEIGHT = 720;
//const unsigned int SCR_WIDTH = 1920;
//const unsigned int SCR_HEIGHT = 1080;
const unsigned int SCR_WIDTH = 2560;
const unsigned int SCR_HEIGHT = 1440;

// camera
Camera camera(glm::vec3(0.0f, 0.0f, 3.0f));
float lastX = (float)SCR_WIDTH / 2.0;
float lastY = (float)SCR_HEIGHT / 2.0;
bool firstMouse = true;

// timing
float deltaTime = 0.0f;
float lastFrame = 0.0f;

// meshes
unsigned int planeVAO;


void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    // glfw: whenever the window size changed (by OS or user resize) this callback function executes
    glViewport(0, 0, width, height);
}


void mouse_callback(GLFWwindow* window, double xPos, double yPos)
{
    if (firstMouse)
    {
        lastX = xPos;
        lastY = yPos;
        firstMouse = false;
    }

    float xOffset = xPos - lastX;
    float yOffset = lastY - yPos;

    lastX = xPos;
    lastY = yPos;

    camera.ProcessMouseMovement(xOffset, yOffset);
}


void scroll_callback(GLFWwindow* window, double xOffSet, double yOffset)
{
    camera.ProcessMouseMovement(xOffSet, yOffset);
}


void processInput(GLFWwindow* window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) glfwSetWindowShouldClose(window, true);

    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) camera.ProcessKeyboard(FORWARD, deltaTime);

    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) camera.ProcessKeyboard(BACKWARD, deltaTime);

    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) camera.ProcessKeyboard(LEFT, deltaTime);

    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) camera.ProcessKeyboard(RIGHT, deltaTime);
        
    if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS) camera.ProcessKeyboard(UP, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS) camera.ProcessKeyboard(DOWN, deltaTime);
}


unsigned int loadTexture(const char* path)
{
    unsigned int textureID;
    glGenTextures(1, &textureID);

    int width, height, nrComponents;
    unsigned char *data = stbi_load(path, &width, &height, &nrComponents, 0);
    if (data)
    {
        GLenum format;
        if (nrComponents == 1) format = GL_RED;
        else if (nrComponents == 3) format = GL_RGB;
        else if (nrComponents == 4) format = GL_RGBA;

        glBindTexture(GL_TEXTURE_2D, textureID);
        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, format == GL_RGBA ? GL_CLAMP_TO_EDGE : GL_REPEAT);
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


unsigned int cubeVAO = 0;
unsigned int cubeVBO = 0;
void renderCube()
{
    if ( cubeVAO == 0 ) {
        float vertices[] = {
                // back face
                -1.0f, -1.0f, -1.0f, 0.0f, 0.0f, -1.0f, 0.0f, 0.0f, // bottom-left
                1.0f, 1.0f, -1.0f, 0.0f, 0.0f, -1.0f, 1.0f, 1.0f, // top-right
                1.0f, -1.0f, -1.0f, 0.0f, 0.0f, -1.0f, 1.0f, 0.0f, // bottom-right
                1.0f, 1.0f, -1.0f, 0.0f, 0.0f, -1.0f, 1.0f, 1.0f, // top-right
                -1.0f, -1.0f, -1.0f, 0.0f, 0.0f, -1.0f, 0.0f, 0.0f, // bottom-left
                -1.0f, 1.0f, -1.0f, 0.0f, 0.0f, -1.0f, 0.0f, 1.0f, // top-left
                // front face
                -1.0f, -1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, // bottom-left
                1.0f, -1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f, 0.0f, // bottom-right
                1.0f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f, // top-right
                1.0f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f, // top-right
                -1.0f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f, // top-left
                -1.0f, -1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, // bottom-left
                // left face
                -1.0f, 1.0f, 1.0f, -1.0f, 0.0f, 0.0f, 1.0f, 0.0f, // top-right
                -1.0f, 1.0f, -1.0f, -1.0f, 0.0f, 0.0f, 1.0f, 1.0f, // top-left
                -1.0f, -1.0f, -1.0f, -1.0f, 0.0f, 0.0f, 0.0f, 1.0f, // bottom-left
                -1.0f, -1.0f, -1.0f, -1.0f, 0.0f, 0.0f, 0.0f, 1.0f, // bottom-left
                -1.0f, -1.0f, 1.0f, -1.0f, 0.0f, 0.0f, 0.0f, 0.0f, // bottom-right
                -1.0f, 1.0f, 1.0f, -1.0f, 0.0f, 0.0f, 1.0f, 0.0f, // top-right
                // right face
                1.0f, 1.0f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f, // top-left
                1.0f, -1.0f, -1.0f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f, // bottom-right
                1.0f, 1.0f, -1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f, // top-right
                1.0f, -1.0f, -1.0f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f, // bottom-right
                1.0f, 1.0f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f, // top-left
                1.0f, -1.0f, 1.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, // bottom-left
                // bottom face
                -1.0f, -1.0f, -1.0f, 0.0f, -1.0f, 0.0f, 0.0f, 1.0f, // top-right
                1.0f, -1.0f, -1.0f, 0.0f, -1.0f, 0.0f, 1.0f, 1.0f, // top-left
                1.0f, -1.0f, 1.0f, 0.0f, -1.0f, 0.0f, 1.0f, 0.0f, // bottom-left
                1.0f, -1.0f, 1.0f, 0.0f, -1.0f, 0.0f, 1.0f, 0.0f, // bottom-left
                -1.0f, -1.0f, 1.0f, 0.0f, -1.0f, 0.0f, 0.0f, 0.0f, // bottom-right
                -1.0f, -1.0f, -1.0f, 0.0f, -1.0f, 0.0f, 0.0f, 1.0f, // top-right
                // top face
                -1.0f, 1.0f, -1.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, // top-left
                1.0f, 1.0f, 1.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f, // bottom-right
                1.0f, 1.0f, -1.0f, 0.0f, 1.0f, 0.0f, 1.0f, 1.0f, // top-right
                1.0f, 1.0f, 1.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f, // bottom-right
                -1.0f, 1.0f, -1.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, // top-left
                -1.0f, 1.0f, 1.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f  // bottom-left
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


void renderScene(const Shader &shader)
{
    //  floor
    glm::mat4 model = glm::mat4(1.0f);
    shader.setMat4("model", model);
    glBindVertexArray(planeVAO);
    glDrawArrays(GL_TRIANGLES, 0, 6);

    //  3 cubes
    model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(0.0f, 1.5f, 0.0f));
    model = glm::scale(model, glm::vec3(0.5f));
    shader.setMat4("model", model);
    renderCube();

    model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(2.0f, 0.0f, 1.0f));
    model = glm::scale(model, glm::vec3(0.5f));
    shader.setMat4("model", model);
    renderCube();

    model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(-1.0f, 0.0f, 2.0f));
    model = glm::rotate(model, glm::radians(60.0f), glm::normalize(glm::vec3(1.0f, 0.0f, 1.0f)));
    model = glm::scale(model, glm::vec3(0.25f));
    shader.setMat4("model", model);
    renderCube();
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
        //  Setup Plane VAO
        glGenVertexArrays(1, &quadVAO);
        glGenBuffers(1, &quadVBO);
        glBindVertexArray(quadVAO);
        glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5*sizeof(float), nullptr);
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5*sizeof(float), (void*)(3*sizeof(float)));
    }
    glBindVertexArray(quadVAO);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    glBindVertexArray(0);
}



int main() {
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    //  glfw window creation
    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "Shadow Maps!", nullptr, nullptr);
    if (window == nullptr)
    {
        std::cout << "Failed to create GLFFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetScrollCallback(window, scroll_callback);

    //  set mouse input
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    //  glad
    if (!gladLoadGLLoader((GLADloadproc) glfwGetProcAddress))
    {
        std::cout << "Failed to Initialize GLAD" << std::endl;
        return -1;
    }

    //  configure global opengl state
    glEnable(GL_DEPTH_TEST);

    //  Build and compile shaders
    Shader shader("../shaders/shadow_mapping.vs", "../shaders/shadow_mapping.fs");
    Shader simpleDepthShader("../shaders/shadow_mapping_depth.vs", "../shaders/shadow_mapping_depth.fs");
    Shader debugDepthQuad("../shaders/debug_quad.vs", "../shaders/debug_quad.fs");

    //  CASCADE SHADOW MAPS VARIABLES
    float lambda = 0.5f;
    float n      = 1.0f;
    //float f      = 100000.0f;
    float f      = 10.0f;        //  The value that works is 7.5f
    const float m      = 6.0f;        //  Number of Cascades
    float C[static_cast<int>(m)+1];
    C[0]         = n;
    C[static_cast<int>(m)+1] = f;


    //  Set up Vertex data and configure vertex attributes
    float planeVertices[] = {
            // positions            // normals         // texcoords
            25.0f, -0.5f,  25.0f,  0.0f, 1.0f, 0.0f,  25.0f,  0.0f,
            -25.0f, -0.5f,  25.0f,  0.0f, 1.0f, 0.0f,   0.0f,  0.0f,
            -25.0f, -0.5f, -25.0f,  0.0f, 1.0f, 0.0f,   0.0f, 25.0f,

            25.0f, -0.5f,  25.0f,  0.0f, 1.0f, 0.0f,  25.0f,  0.0f,
            -25.0f, -0.5f, -25.0f,  0.0f, 1.0f, 0.0f,   0.0f, 25.0f,
            25.0f, -0.5f, -25.0f,  0.0f, 1.0f, 0.0f,  25.0f, 25.0f
    };
    // plane VAO
    unsigned int planeVBO;
    glGenVertexArrays(1, &planeVAO);
    glGenBuffers(1, &planeVBO);
    glBindVertexArray(planeVAO);
    glBindBuffer(GL_ARRAY_BUFFER, planeVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(planeVertices), planeVertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
    glBindVertexArray(0);

    //  load textures
    unsigned int woodTexture = loadTexture(std::filesystem::path("../resources/textures/wood.png").c_str());

    //  Configure Depth Map FBO
    const unsigned int SHADOW_WIDTH = 2048, SHADOW_HEIGHT = 2048;
    GLuint depthMapFBO[static_cast<GLint>(m)];
    glGenFramebuffers(static_cast<GLint>(m), depthMapFBO);
    //unsigned int depthMapFBO;
    //glGenFramebuffers(1, &depthMapFBO);

    //  Create depth texture
    GLuint depthMap[static_cast<GLint>(m)];
    glGenTextures(static_cast<GLint>(m), depthMap);
    for ( unsigned int i = 0; i < static_cast<unsigned int>(m); ++i ) {
        glBindTexture(GL_TEXTURE_2D, depthMap[i]);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, SHADOW_WIDTH, SHADOW_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT,
                     nullptr);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
        float borderColor[] = {1.0, 1.0, 1.0, 1.0};
        glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);

        //  Attach depth texture as FBOs depth buffer
        glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO[i]);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthMap[i], 0);
        glDrawBuffer(GL_NONE);
        glReadBuffer(GL_NONE);
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }

    //  Shader Configuration
    shader.use();
    shader.setInt("diffuseTexture", 0);
    shader.setInt("shadowMap", 1);
    debugDepthQuad.use();
    //debugDepthQuad.setInt("depthMap", 0);

    //  Lighting Info
    glm::vec3 lightPos(-2.0f, 4.0f, -1.0f);

    std::vector<float> cascadesSplit;

    for (int i = 0; i < static_cast<int>(m); ++i) {
        float Cuniform = n + ((f - n) * ((static_cast<float>(i) /*+ 1.0f*/) / m));
        float Clogarithmic = n * powf(f / n, (static_cast<float>(i) /*+ 1.0f*/) / m);
        float c = lambda * Cuniform + (1 - lambda) * Clogarithmic;
        C[i] = c;
        std::cout << "C_" << i << " = " << c << std::endl;
        cascadesSplit.push_back(c);
    }

    float near_plane = n, far_plane = f;
    float aspectRatio = static_cast<float>(SCR_WIDTH)/static_cast<float>(SCR_HEIGHT);

    //  Render Loop
    while (!glfwWindowShouldClose(window))
    {
        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        //  Input
        processInput(window);

        // change light position over time
        lightPos.x = sin(glfwGetTime()) * 3.0f;
        lightPos.z = cos(glfwGetTime()) * 2.0f;
        lightPos.y = 5.0f + cos(glfwGetTime()) * 1.0f;

        //  Render
        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glm::mat4 cameraViewMatrix = camera.GetViewMatrix();

        glm::mat4 lightProjection, lightView;
        glm::mat4 lightSpaceMatrix;
        std::vector<glm::mat4> lightMatrixCascArr;

        glm::vec3 cameraTarget = glm::vec3(0.0f, 0.0f, 0.0f);
        glm::vec3 cameraDirection = glm::normalize(camera.Position);
        glm::vec3 up(0.0f, 1.0f, 0.0f);
        glm::vec3 right = glm::cross(cameraDirection, up);
        glm::vec3 center = camera.Position;

        for (int i = 0; i < static_cast<int>(m); ++i)
        {
            glm::vec3 centerFar = center + cameraDirection * C[i+1];
            glm::vec3 centerNear = center + cameraDirection * C[i];
            right = glm::normalize(right);
            up = glm::normalize(glm::cross(right, cameraDirection));

            float frustrumHeightNear = 2.0f * C[i] * tanf(glm::radians(camera.Zoom));
            float frustrumWidthNear = frustrumHeightNear * (static_cast<float>(SCR_WIDTH) / static_cast<float>(SCR_HEIGHT));
            //glm::vec3 centerNear = camera.Position + (C[i]*(camera.Front));
            float frustrumHeightFar = 2.0f * C[i] * tanf(glm::radians(camera.Zoom));
            float frustrumWidthFar = frustrumHeightFar * (static_cast<float>(SCR_WIDTH) / static_cast<float>(SCR_HEIGHT));
            //glm::vec3 centerFar = camera.Position + (C[i+1]*(camera.Front));

            //glm::vec3 ntl = centerNear + (camera.Up * (frustrumHeightNear/2.0f)) - (camera.Right * (frustrumWidthNear)/2.0f);
            glm::vec3 ntl = centerNear + (up * (frustrumHeightNear/2.0f)) - (right * (frustrumWidthNear)/2.0f);
            //glm::vec3 ntr = centerNear + (camera.Up * (frustrumHeightNear/2.0f)) + (camera.Right * (frustrumWidthNear)/2.0f);
            glm::vec3 ntr = centerNear + (up * (frustrumHeightNear/2.0f)) + (right * (frustrumWidthNear)/2.0f);
            //glm::vec3 nbl = centerNear - (camera.Up * (frustrumHeightNear/2.0f)) - (camera.Right * (frustrumWidthNear)/2.0f);
            glm::vec3 nbl = centerNear - (up * (frustrumHeightNear/2.0f)) - (right * (frustrumWidthNear)/2.0f);
            //glm::vec3 nbr = centerNear - (camera.Up * (frustrumHeightNear/2.0f)) + (camera.Right * (frustrumWidthNear)/2.0f);
            glm::vec3 nbr = centerNear - (up * (frustrumHeightNear/2.0f)) + (right * (frustrumWidthNear)/2.0f);

            //glm::vec3 ftl = centerFar + (camera.Up * (frustrumHeightFar/2.0f)) - (camera.Right * (frustrumWidthFar)/2.0f);
            glm::vec3 ftl = centerFar + (up * (frustrumHeightFar/2.0f)) - (right * (frustrumWidthFar)/2.0f);
            //glm::vec3 ftr = centerFar + (camera.Up * (frustrumHeightFar/2.0f)) + (camera.Right * (frustrumWidthFar)/2.0f);
            glm::vec3 ftr = centerFar + (up * (frustrumHeightFar/2.0f)) + (right * (frustrumWidthFar)/2.0f);
            //glm::vec3 fbl = centerFar - (camera.Up * (frustrumHeightFar/2.0f)) - (camera.Right * (frustrumWidthFar)/2.0f);
            glm::vec3 fbl = centerFar - (up * (frustrumHeightFar/2.0f)) - (right * (frustrumWidthFar)/2.0f);
            //glm::vec3 fbr = centerFar - (camera.Up * (frustrumHeightFar/2.0f)) + (camera.Right * (frustrumWidthFar)/2.0f);
            glm::vec3 fbr = centerFar - (up * (frustrumHeightFar/2.0f)) + (right * (frustrumWidthFar)/2.0f);

            //  Frustrum Corners
            glm::vec4 bboxCorners[8];
            bboxCorners[0] = glm::vec4(nbl, 0);
            bboxCorners[1] = glm::vec4(nbr, 0);
            bboxCorners[2] = glm::vec4(ntl, 0);
            bboxCorners[3] = glm::vec4(ntr, 0);
            bboxCorners[4] = glm::vec4(fbl, 0);
            bboxCorners[5] = glm::vec4(fbr, 0);
            bboxCorners[6] = glm::vec4(ftl, 0);
            bboxCorners[7] = glm::vec4(ftr, 0);

            //  Transform corner vector by the cameras inverse view projection model matrix
            glm::mat4 projectionMatr = glm::perspective( glm::radians(camera.Zoom), aspectRatio, near_plane, far_plane);
            glm::mat4 invViewProj = glm::inverse(projectionMatr*cameraViewMatrix);
            for (auto &bboxCorner : bboxCorners)
            {
                //bboxCorner = glm::inverse(cameraViewMatrix) * bboxCorner;
                bboxCorner = invViewProj * bboxCorner;
            }

            //glm::vec3 minVctr = glm::vec3(-frustrumWidthNear / 2, -frustrumHeightNear / 2, C[0]);
            glm::vec3 minVctr = glm::vec3(INFINITY, INFINITY, INFINITY);
            //glm::vec3 maxVctr = glm::vec3(frustrumWidthFar / 2, frustrumHeightFar / 2, C[i + 1]);
            glm::vec3 maxVctr = glm::vec3(-INFINITY, -INFINITY, -INFINITY);

            for (const auto &bboxCorner : bboxCorners) {
                if (minVctr.x > bboxCorner.x) minVctr.x = bboxCorner.x;
                if (minVctr.y > bboxCorner.y) minVctr.y = bboxCorner.y;
                if (minVctr.z > bboxCorner.z) minVctr.z = bboxCorner.z;
                if (maxVctr.x < bboxCorner.x) maxVctr.x = bboxCorner.x;
                if (maxVctr.y < bboxCorner.y) maxVctr.y = bboxCorner.y;
                if (maxVctr.z < bboxCorner.z) maxVctr.z = bboxCorner.z;
            }

            float scaleX = 2.0f / (maxVctr.x - minVctr.x);
            float scaleY = 2.0f / (maxVctr.y - minVctr.y);
            float scaleZ = 1.0f / (maxVctr.z - minVctr.z);

            float offsetX = -0.5f * (maxVctr.x + minVctr.x) * scaleX;
            float offsetY = -0.5f * (maxVctr.y + minVctr.y) * scaleY;
            float offsetZ = -minVctr.z * scaleZ;

            glm::mat4 cropMatrix = {scaleX, 0.0f, 0.0f, offsetX,
                                    0.0f, scaleY, 0.0f, offsetY,
                                    0.0f, 0.0f, scaleZ, offsetZ,
                                    0.0f, 0.0f, 0.0f, 1.0f};

            //  1.  Render Depth of Scene to Texture from lights perspective
            lightProjection = glm::ortho(-10.0f, 10.0f, -10.0f, 10.0f, near_plane, far_plane);
            //lightProjection = glm::ortho(-10.0f, 10.0f, -10.0f, 10.0f, C[i], C[i+1]);
            //lightProjection = glm::ortho(-1.0f, 1.0f, -1.0f, 1.0f, n, f);
            //lightProjection = glm::ortho(minVctr.x, maxVctr.x, minVctr.y, maxVctr.y, minVctr.z, maxVctr.z);
            lightView = glm::lookAt(lightPos, glm::vec3(0.0f), glm::vec3(0.0, 1.0, 0.0));
            //lightView = glm::lookAt(lightPos, frustumCenter, glm::vec3(0.0f, 0.0f, 1.0f));
            lightSpaceMatrix = cropMatrix * lightProjection * lightView;

            lightMatrixCascArr.push_back(lightSpaceMatrix);

            //  Render Scene from lights point of view
            simpleDepthShader.use();
            simpleDepthShader.setMat4("lightSpaceMatrix", lightSpaceMatrix);
            //simpleDepthShader.setMultMat4("lightSpaceMatrix", lightMatrixCascArr, static_cast<int>(m));

            glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);
            glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO[i]);
            glClear(GL_DEPTH_BUFFER_BIT);
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, woodTexture);
            renderScene(simpleDepthShader);
            glBindFramebuffer(GL_FRAMEBUFFER, 0);
        }
            //  Reset Viewport
            glViewport(0, 0, SCR_WIDTH, SCR_HEIGHT);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

            //  2.  Render Scene as normal using the generated depth/shadow map
            glViewport(0, 0, SCR_WIDTH, SCR_HEIGHT);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
            shader.use();
            glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom),
                                                    (float) SCR_WIDTH / (float) SCR_HEIGHT,
                                                    1.0f, 100.0f);
            glm::mat4 view = camera.GetViewMatrix();
            shader.setFloatArr("cascadesSplit", cascadesSplit);
            shader.setMat4("projection", projection);
            shader.setMat4("view", view);
            //  Set Light Uniforms
            shader.setVec3("viewPos", camera.Position);
            shader.setVec3("lightPos", lightPos);
            shader.setMultMat4("lightSpaceMatrix", lightMatrixCascArr, static_cast<int>(m));
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, woodTexture);
            for (int i = 0; i < static_cast<int>(m); ++i)
            {
                glActiveTexture(GL_TEXTURE1+i);
                glBindTexture(GL_TEXTURE_2D, depthMap[i]);
            }
            renderScene(shader);

            //  Render Depth map to Quad for Visual Debugging
            debugDepthQuad.use();
            debugDepthQuad.setFloat("near_plane", near_plane);
            debugDepthQuad.setFloat("far_plane", far_plane);
            for (int i = 0; i < static_cast<int>(m); ++i)
            {
                glActiveTexture(GL_TEXTURE1+i);
                glBindTexture(GL_TEXTURE_2D, depthMap[i]);
            }
            //renderQuad();

        //  glfw: swap buffers and poll IO events
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    //  De Allocate all resources once they have outlived their purpose
    glDeleteVertexArrays(1, &planeVAO);
    glDeleteBuffers(1, &planeVBO);
    glfwTerminate();

    return 0;
}
