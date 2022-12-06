
#define GLEW_STATIC
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "Camera.h"
#include "Material.h"
#include "LightDirectional.h"
#include "LightPoint.h"
#include "LightSpot.h"
#include "Model.h"
#include "ProjectMove.h"
#include "SkeletalAnimation.h"
#include "myObject.h"
#include "Shape.h"
#include "Piano.h"

using namespace glm;
void processInput(GLFWwindow* window, float& , ProjectMove* project_move, MyObject * myobj, Shape* shape);
void mouse_callback(GLFWwindow* window, double xPos, double yPos);
float deltX = 0.0f;
float deltZ = 0.0f;
float rotateAngle = 0.0f;
float Scale = 1.0f;
const float scaleSpeed = 0.5f;
int MODEL_NUM;
#pragma region Camera declare
Camera camera(glm::vec3(0, 15.0f, 40.0f), glm::radians(0.0f), glm::radians(180.0f), glm::vec3(0, 1.0f, 0));
//Project Move

#pragma endregion

#pragma region Light declare

LightDirectional lightD(glm::vec3(0.0f, 30.0f, 0.0f),
    glm::vec3(glm::radians(45.f), glm::radians(45.f), 0),
    glm::vec3(1.0f, 1.0f, 1.0f));

LightPoint lightDFront (glm::vec3(0.0f, 0.0f, 50.0f),
    glm::vec3(glm::radians(45.f), glm::radians(45.f), 0), 
    glm::vec3(1.0f,1.0f, 1.0f));

LightPoint lightDBack(glm::vec3(0.0f, 0.0f, -50.0f),
    glm::vec3(glm::radians(45.f), glm::radians(45.f), 0),
    glm::vec3(1.0f, 1.0f, 1.0f));

LightPoint lightDLeft(glm::vec3(50.0f, 0.0f, 0.0f),
    glm::vec3(glm::radians(45.f), glm::radians(45.f), 0),
    glm::vec3(1.0f, 1.0f, 1.0f));

LightPoint lightDRight(glm::vec3(-50.0f, 0.0f, 0.0f),
    glm::vec3(glm::radians(45.f), glm::radians(45.f), 0),
    glm::vec3(1.0f, 1.0f, 1.0f));

LightPoint lightP0 (glm::vec3(0.0f, 0.0f, 0.0f), 
    glm::vec3(glm::radians(45.f), glm::radians(45.f), 0), 
    glm::vec3(1.0f, 1.0f, 1.0f));
LightPoint lightP1 (glm::vec3(0.0f, 100.0f, 0.0f),
    glm::vec3(glm::radians(45.f), glm::radians(45.f), 0),
    glm::vec3(1.0f, 1.0f, 1.0f));
LightPoint lightP2 (glm::vec3(0.0f, 3.0f, 1.0f),
    glm::vec3(glm::radians(45.f), glm::radians(45.f), 0),
    glm::vec3(1.0f, 1.0f, 1.0f));
LightPoint lightP3(glm::vec3(1.0f, 1.0f, 1.0f),
    glm::vec3(glm::radians(45.f), glm::radians(45.f), 0),
    glm::vec3(1.0f, 1.0f, 1.0f));
LightSpot lightS (glm::vec3(0.0f, 5.0f, 0.0f), 
    glm::vec3(glm::radians(90.0f),0, 0),
    glm::vec3(1.0f, 1.0f, 1.0f));
#pragma endregion

#pragma region Input Declare
float lastX;
float lastY;
bool firstMouse = true;


void mouse_callback(GLFWwindow* window, double xPos, double yPos)
{
    if (firstMouse == true) {
        lastX = xPos;
        lastY = yPos;
        firstMouse = false;
    }
    float deltaX, deltaY;
    deltaX = xPos - lastX;
    deltaY = yPos - lastY;

    lastX = xPos;
    lastY = yPos;
    //printf("%f \n", xPos);
    camera.processMouseMovement(deltaX, deltaY);
}


#pragma endregion

#pragma region Modeal Data

float unitLineVertices[] = {
      -1.0f, 0.0f, 0.0f, 1.0f, 1.0f, 0.0f, // pos * 3, color * 3
      1.0f, 0.0f, 0.0f, 1.0f, 1.0f, 0.0f
};
float vertices[] = {
    // positions          // normals           // texture coords
    -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f,  0.0f,
     0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f,  0.0f,
     0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f,  1.0f,
     0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f,  1.0f,
    -0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f,  1.0f,
    -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f,  0.0f,

    -0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f,  0.0f,
     0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  1.0f,  0.0f,
     0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  1.0f,  1.0f,
     0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  1.0f,  1.0f,
    -0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f,  1.0f,
    -0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f,  0.0f,

    -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f,  0.0f,
    -0.5f,  0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  1.0f,  1.0f,
    -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
    -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
    -0.5f, -0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  0.0f,  0.0f,
    -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f,  0.0f,

     0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f,  0.0f,
     0.5f,  0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  1.0f,  1.0f,
     0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
     0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
     0.5f, -0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  0.0f,  0.0f,
     0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f,  0.0f,

    -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f,  1.0f,
     0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  1.0f,  1.0f,
     0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f,  0.0f,
     0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f,  0.0f,
    -0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  0.0f,  0.0f,
    -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f,  1.0f,

    -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f,  1.0f,
     0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  1.0f,  1.0f,
     0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f,  0.0f,
     0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f,  0.0f,
    -0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  0.0f,  0.0f,
    -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f,  1.0f
};

glm::vec3 cubePositions[] = {
  glm::vec3(0.0f,  0.0f,  0.0f),
  glm::vec3(2.0f,  5.0f, -15.0f),
  glm::vec3(-1.5f, -2.2f, -2.5f),
  glm::vec3(-3.8f, -2.0f, -12.3f),
  glm::vec3(2.4f, -0.4f, -3.5f),
  glm::vec3(-1.7f,  3.0f, -7.5f),
  glm::vec3(1.3f, -2.0f, -2.5f),
  glm::vec3(1.5f,  2.0f, -2.5f),
  glm::vec3(1.5f,  0.2f, -1.5f),
  glm::vec3(-1.3f,  1.0f, -1.5f)
};

#pragma endregion


unsigned int loadImageToGPU(const char* filename, GLint internalformat, GLenum format, int textureSlot)
{
    stbi_set_flip_vertically_on_load(true);
    unsigned int TexBuffer;
    glGenTextures(1, &TexBuffer);
    glActiveTexture(GL_TEXTURE0 + textureSlot);
    glBindTexture(GL_TEXTURE_2D, TexBuffer);

    int width, height, nrChannel;
    unsigned char* data = stbi_load(filename, &width, &height, &nrChannel, 0);
    if (data) {
        glTexImage2D(GL_TEXTURE_2D, 0, internalformat, width, height, 0, format, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
    }
    else {
        printf("load image fail\n");
    }
    stbi_image_free(data);
    return TexBuffer;
}

//// Textured Cube model
//struct TexturedColoredVertex
//{
//    TexturedColoredVertex(vec3 _position, vec3 _color, vec2 _uv)
//        : position(_position), color(_color), uv(_uv) {}
//
//    vec3 position;
//    vec3 color;
//    vec2 uv;
//};
// Textured Cube model
const TexturedColoredVertex texturedCubeVertexArray[] = {
    // position,                normal                       UV
TexturedColoredVertex(vec3(-0.5f,-0.5f,-0.5f),   vec3(-1.0f, 0.0f, 0.0f),   vec2(0.0f, 0.0f)),   //left - red
TexturedColoredVertex(vec3(-0.5f,-0.5f, 0.5f),   vec3(-1.0f, 0.0f, 0.0f),   vec2(0.0f, 1.0f)),
TexturedColoredVertex(vec3(-0.5f, 0.5f, 0.5f),   vec3(-1.0f, 0.0f, 0.0f),   vec2(1.0f, 1.0f)),

TexturedColoredVertex(vec3(-0.5f,-0.5f,-0.5f),   vec3(-1.0f, 0.0f, 0.0f),   vec2(0.0f, 0.0f)),
TexturedColoredVertex(vec3(-0.5f, 0.5f, 0.5f),   vec3(-1.0f, 0.0f, 0.0f),   vec2(1.0f, 1.0f)),
TexturedColoredVertex(vec3(-0.5f, 0.5f,-0.5f),   vec3(-1.0f, 0.0f, 0.0f),   vec2(1.0f, 0.0f)),

TexturedColoredVertex(vec3(0.5f, 0.5f,-0.5f),    vec3(0.0f, 0.0f, -1.0f),   vec2(1.0f, 1.0f)),    // far - blue
TexturedColoredVertex(vec3(-0.5f,-0.5f,-0.5f),   vec3(0.0f, 0.0f, -1.0f),   vec2(0.0f, 0.0f)),
TexturedColoredVertex(vec3(-0.5f, 0.5f,-0.5f),   vec3(0.0f, 0.0f, -1.0f),   vec2(0.0f, 1.0f)),

TexturedColoredVertex(vec3(0.5f, 0.5f,-0.5f),    vec3(0.0f, 0.0f, -1.0f),   vec2(1.0f, 1.0f)),
TexturedColoredVertex(vec3(0.5f,-0.5f,-0.5f),    vec3(0.0f, 0.0f, -1.0f),   vec2(1.0f, 0.0f)),
TexturedColoredVertex(vec3(-0.5f,-0.5f,-0.5f),   vec3(0.0f, 0.0f, -1.0f),   vec2(0.0f, 0.0f)),

TexturedColoredVertex(vec3(0.5f,-0.5f, 0.5f),    vec3(0.0f,-1.0f, 0.0f),    vec2(1.0f, 1.0f)),    // bottom - turquoise
TexturedColoredVertex(vec3(-0.5f,-0.5f,-0.5f),   vec3(0.0f,-1.0f, 0.0f),    vec2(0.0f, 0.0f)),
TexturedColoredVertex(vec3(0.5f,-0.5f,-0.5f),    vec3(0.0f,-1.0f, 0.0f),    vec2(1.0f, 0.0f)),

TexturedColoredVertex(vec3(0.5f,-0.5f, 0.5f),    vec3(0.0f,-1.0f, 0.0f),    vec2(1.0f, 1.0f)),
TexturedColoredVertex(vec3(-0.5f,-0.5f, 0.5f),   vec3(0.0f,-1.0f, 0.0f),    vec2(0.0f, 1.0f)),
TexturedColoredVertex(vec3(-0.5f,-0.5f,-0.5f),   vec3(0.0f,-1.0f, 0.0f),    vec2(0.0f, 0.0f)),

TexturedColoredVertex(vec3(-0.5f, 0.5f, 0.5f),   vec3(0.0f, 0.0f, 1.0f),    vec2(0.0f, 1.0f)),// near - green
TexturedColoredVertex(vec3(-0.5f,-0.5f, 0.5f),   vec3(0.0f, 0.0f, 1.0f),    vec2(0.0f, 0.0f)),
TexturedColoredVertex(vec3(0.5f,-0.5f, 0.5f),    vec3(0.0f, 0.0f, 1.0f),    vec2(1.0f, 0.0f)),

TexturedColoredVertex(vec3(0.5f, 0.5f, 0.5f),    vec3(0.0f, 0.0f, 1.0f),    vec2(1.0f, 1.0f)),
TexturedColoredVertex(vec3(-0.5f, 0.5f, 0.5f),   vec3(0.0f, 0.0f, 1.0f),    vec2(0.0f, 1.0f)),
TexturedColoredVertex(vec3(0.5f,-0.5f, 0.5f),    vec3(0.0f, 0.0f, 1.0f),    vec2(1.0f, 0.0f)),

TexturedColoredVertex(vec3(0.5f, 0.5f, 0.5f),    vec3(1.0f, 0.0f, 0.0f),    vec2(1.0f, 1.0f)),  // right - purple
TexturedColoredVertex(vec3(0.5f,-0.5f,-0.5f),    vec3(1.0f, 0.0f, 0.0f),    vec2(0.0f, 0.0f)),
TexturedColoredVertex(vec3(0.5f, 0.5f,-0.5f),    vec3(1.0f, 0.0f, 0.0f),    vec2(1.0f, 0.0f)),

TexturedColoredVertex(vec3(0.5f,-0.5f,-0.5f),    vec3(1.0f, 0.0f, 0.0f),    vec2(0.0f, 0.0f)),
TexturedColoredVertex(vec3(0.5f, 0.5f, 0.5f),    vec3(1.0f, 0.0f, 0.0f),    vec2(1.0f, 1.0f)),
TexturedColoredVertex(vec3(0.5f,-0.5f, 0.5f),    vec3(1.0f, 0.0f, 0.0f),    vec2(0.0f, 1.0f)),

TexturedColoredVertex(vec3(0.5f, 0.5f, 0.5f),    vec3(0.0f, 1.0f, 0.0f),    vec2(1.0f, 1.0f)),// top - yellow
TexturedColoredVertex(vec3(0.5f, 0.5f,-0.5f),    vec3(0.0f, 1.0f, 0.0f),    vec2(1.0f, 0.0f)),
TexturedColoredVertex(vec3(-0.5f, 0.5f,-0.5f),   vec3(0.0f, 1.0f, 0.0f),    vec2(0.0f, 0.0f)),

TexturedColoredVertex(vec3(0.5f, 0.5f, 0.5f),    vec3(0.0f, 1.0f, 0.0f),    vec2(1.0f, 1.0f)),
TexturedColoredVertex(vec3(-0.5f, 0.5f,-0.5f),   vec3(0.0f, 1.0f, 0.0f),    vec2(0.0f, 0.0f)),
TexturedColoredVertex(vec3(-0.5f, 0.5f, 0.5f),   vec3(0.0f, 1.0f, 0.0f),    vec2(0.0f, 1.0f))
};

int createTexturedCubeVertexArrayObject()
{
    // Create a vertex array
    GLuint vertexArrayObject;
    glGenVertexArrays(1, &vertexArrayObject);
    glBindVertexArray(vertexArrayObject);

    // Upload Vertex Buffer to the GPU, keep a reference to it (vertexBufferObject)
    GLuint vertexBufferObject;
    glGenBuffers(1, &vertexBufferObject);
    glBindBuffer(GL_ARRAY_BUFFER, vertexBufferObject);
    glBufferData(GL_ARRAY_BUFFER, sizeof(texturedCubeVertexArray), texturedCubeVertexArray, GL_STATIC_DRAW);

    glVertexAttribPointer(0,                   // attribute 0 matches aPos in Vertex Shader
        3,                   // size
        GL_FLOAT,            // type
        GL_FALSE,            // normalized?
        sizeof(TexturedColoredVertex), // stride - each vertex contain 2 vec3 (position, color)
        (void*)0             // array buffer offset
    );
    glEnableVertexAttribArray(0);


    glVertexAttribPointer(1,                            // attribute 1 matches aColor in Vertex Shader
        3,
        GL_FLOAT,
        GL_FALSE,
        sizeof(TexturedColoredVertex),
        (void*)sizeof(vec3)      // color is offseted a vec3 (comes after position)
    );
    glEnableVertexAttribArray(1);

    glVertexAttribPointer(2,                            // attribute 2 matches aUV in Vertex Shader
        2,
        GL_FLOAT,
        GL_FALSE,
        sizeof(TexturedColoredVertex),
        (void*)(2 * sizeof(vec3))      // uv is offseted by 2 vec3 (comes after position and color)
    );
    glEnableVertexAttribArray(2);

    return vertexArrayObject;
}

GLuint loadTexture(const char* filename)
{
    // Step1 Create and bind textures
    GLuint textureId = 0;
    glGenTextures(1, &textureId);
    assert(textureId != 0);


    glBindTexture(GL_TEXTURE_2D, textureId);

    // Step2 Set filter parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    // Step3 Load Textures with dimension data
    int width, height, nrChannels;
    unsigned char* data = stbi_load(filename, &width, &height, &nrChannels, 0);
    if (!data)
    {
        std::cerr << "Error::Texture could not load texture file:" << filename << std::endl;
        return 0;
    }

    // Step4 Upload the texture to the PU
    GLenum format = 0;
    if (nrChannels == 1)
        format = GL_RED;
    else if (nrChannels == 3)
        format = GL_RGB;
    else if (nrChannels == 4)
        format = GL_RGBA;
    glTexImage2D(GL_TEXTURE_2D, 0, format, width, height,
        0, format, GL_UNSIGNED_BYTE, data);

    // Step5 Free resources
    stbi_image_free(data);
    glBindTexture(GL_TEXTURE_2D, 0);
    return textureId;
}


int main(int argc, char* argv[])
{
    
    std::string exePath = argv[0];

    std::cout << exePath.substr(0, exePath.find_last_of('\\') ) + "\\" + "\\model\\Olaf.obj" << std::endl;
    
#pragma region Open window
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    
    //Open GLFW Window
    GLFWwindow* window = glfwCreateWindow(1024, 768, "Exercise", NULL, NULL);
    if (window == NULL) {
        printf("open window fail");
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    glfwSetCursorPosCallback(window, mouse_callback);
    //Init GLEW
    glewExperimental = true;
    if (glewInit() != GLEW_OK) {
        printf("init glew fail");
        glfwTerminate();
        return -1;
    }

    glViewport(0, 0, 1024, 768);
    glEnable(GL_DEPTH_TEST);
    #pragma endregion
    glm::mat4 modelMat;
    mat4 manModelMat;
    //modelMat = glm::rotate(modelMat, (float)glfwGetTime() * glm::radians(50.0f), glm::vec3(0.5f, 1.0f, 0.0f));
    glm::mat4 projMat;
    projMat = glm::perspective(glm::radians(45.0f), 1024.0f / 768.0f, 0.01f, 800.0f);
    glm::mat4 viewMat;
#pragma endregion
    float lastFrameTime = glfwGetTime();

    #pragma region Inite Shader
    Shader* myShader = new Shader("vertexSource.vert", "fragmentSource.frag");
    #pragma endregion

#pragma region Init Material
    Material* myMaterial = new Material(myShader,
        loadImageToGPU("grass.jpg", GL_RGB, GL_RGB, Shader::DIFFUSE),
        loadImageToGPU("grass.jpg", GL_RGB, GL_RGB, Shader::SPECULAR),
        glm::vec3(1.0f, 1.0f, 1.0f),
        64.0f);
    myMaterial->emission = loadImageToGPU("matrix.jpg", GL_RGB, GL_RGB, Shader::EMISSION);
#pragma endregion

   
   

    #pragma region Init & Load VAO, VBO
    Model museum_model(exePath.substr(0, exePath.find_last_of('\\')) + "\\" + "\\model1\\museum.obj");
    Model man_model(exePath.substr(0, exePath.find_last_of('\\')) + "\\" + "\\model\\backpack.obj");

    //Zhang yinan
    Shader shaderMyObject("assets/shaders/myObject.vert", "assets/shaders/myObject.frag");
    MyObject* myObject = new MyObject(window);
    //Bo Zhang
    ProjectMove* project_move = new ProjectMove();
    //Skeletal Animation
    SkeletalAnimation* skeletal_animation = new SkeletalAnimation();

    //teddy
    Shape* shape = new Shape(camera);
    //Grid
    unsigned int grassMap;
    grassMap = loadTexture("grass.jpg");
    std::cout << "grassMap :" << grassMap << std::endl;
    int groundVao = createTexturedCubeVertexArrayObject();


//Piano Part
#pragma region load textures and shaders
    // Load Textures
    GLuint groundTextureID = loadTexture("textures\\floor.png");
    GLuint blackKey = loadTexture("textures\\blackpiano.jpg");
    GLuint whiteKey = loadTexture("textures\\white.jpg");

    std::string shaderPathPrefix = "shaders\\";
    //shader used to draw shadow
    GLuint shaderShadow = loadSHADER(shaderPathPrefix + "shadow_vertex.vert", shaderPathPrefix + "shadow_fragment.frag");
    // shader used to draw cube without texture
    GLuint vertex_frag_Shader = loadSHADER(shaderPathPrefix + "vertexShader.glsl", shaderPathPrefix + "fragmentShader.glsl");
    // shader shaderscene
    GLuint shaderScene = loadSHADER(shaderPathPrefix + "scene_vertex.vert", shaderPathPrefix + "scene_fragment.frag");
    // ground shader
    GLuint groundShader = loadSHADER(shaderPathPrefix + "ground_vertex.vert", shaderPathPrefix + "ground_frag.frag");
#pragma endregion load textures and shaders
#pragma region create and set projection matrix and viewmatrix
    // Set projection matrix for shader, this won't change
    mat4 projectionMatrix = glm::perspective(70.0f,           // field of view in degrees
        WIDTH * 1.0f / HEIGHT, // aspect ratio
        0.01f, 400.0f);  // near and far (near > 0)

// Set initial view matrix on both shaders
    mat4 viewMatrix = lookAt(camera.Position,                // eye
        camera.Position + camera.Forward, // center
        camera.WorldUp);                     // up

// Set projection matrix on both shaders
    SetUniformMat4(shaderScene, "projection_matrix", projMat);
    SetUniformMat4(groundShader, "projection_matrix", projMat);

    // Set view matrix on both shaders
    SetUniformMat4(shaderScene, "view_matrix", viewMatrix);
    SetUniformMat4(groundShader, "view_matrix", viewMatrix);

#pragma endregion projection and viewmatrix
    // set vao
    // Define and upload geometry to the GPU here ...
    int sphereAO = createSphereVAO(sphereVertex, sizeof(sphereVertex)); // sphere

    int groundAO = createCubeNormalsUvVAO(posNormUvCubeArray, sizeof(posNormUvCubeArray)); // ground with normals, uv

#pragma region depth map set up
    // set depth map texture
    GLuint depth_map_texture;
    glGenTextures(1, &depth_map_texture);
    // Bind the texture so the next glTex calls affect it
    glBindTexture(GL_TEXTURE_2D, depth_map_texture);
    // Create the texture and specify it's attributes, including width
    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, DEPTH_MAP_TEXTURE_SIZE, DEPTH_MAP_TEXTURE_SIZE, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
    // Set texture sampler parameters.
  // The two calls below tell the texture sampler inside the shader how 
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    // The two calls below tell the texture sampler inside the shader 
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    // set frame buffer used to store depth map texture
    GLuint depth_map_fbo;
    glGenFramebuffers(1, &depth_map_fbo);
    glBindFramebuffer(GL_FRAMEBUFFER, depth_map_fbo);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depth_map_texture, 0);
    glDrawBuffer(GL_NONE);

#pragma endregion depth map set up
    #pragma endregion
     
    #pragma region Init and Load Textures
    //ÎÆÀí
    
    /*unsigned int TexBufferA;
    TexBufferA = loadImageToGPU("container.jpg", GL_RGB, GL_RGB, 0);
    
    unsigned int TexBufferB;
    TexBufferB = loadImageToGPU("awesomeface.png", GL_RGBA, GL_RGBA, 3);*/
    #pragma endregion
    #pragma region Prepare MVP matrices
    //calculate transformation
    //glm::mat4 trans;
    //trans = glm::translate(trans, glm::vec3(0.5f, -0.5f, 0));
    //trans = glm::rotate(trans, (float)glfwGetTime(), glm::vec3(0, 0, 1.0f));
    //trans = glm::scale(trans, glm::vec3(0.5f, 0.5f, 0.5f));
    
    //Ö÷Ñ­»·
    while (!glfwWindowShouldClose(window)) {
        
        float dt = glfwGetTime() - lastFrameTime;
        lastFrameTime += dt;
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        
        viewMat = camera.getViewMatrix();


       
       /// <summary>
       /// Shape
       /// </summary>
       /// <param name="argc"></param>
       /// <param name="argv"></param>
       /// <returns></returns>
        shape->Draw(camera, dt, viewMat, window);

        /// <summary>
       /// Skeletal Animation
       /// </summary>
       /// <param name="argc"></param>
       /// <param name="argv"></param>
       /// <returns></returns>

        skeletal_animation->Draw(viewMat);
        
        
        /// <summary>
       /// Piano
       /// </summary>
       /// <param name="argc"></param>
       /// <param name="argv"></param>
       /// <returns></returns>
        //Piano
#pragma region set and pass light parameters, shadow, textures to shader
        // light parameters
        vec3 lightPosition = lampLightPos;// the location of the light in 3D space
           //vec3(sinf(glfwGetTime() * 3.0f * 3.141592f), sinf(glfwGetTime() * 3.141592f), cosf(glfwGetTime() * 3.141592f));
        vec3 lightFocus(0.0, 0.0, -1.0);      // the point in 3D space the light "looks" at
        vec3 lightDirection = normalize(lightFocus - lightPosition);

        float lightNearPlane = 1.0f;
        float lightFarPlane = 180.0f;

        mat4 lightProjectionMatrix = frustum(-1.0f, 1.0f, -1.0f, 1.0f, lightNearPlane, lightFarPlane);
        //perspective(20.0f, (float)DEPTH_MAP_TEXTURE_SIZE / (float)DEPTH_MAP_TEXTURE_SIZE, lightNearPlane, lightFarPlane);
        mat4 lightViewMatrix = lookAt(lightPosition, lightFocus, vec3(0.0f, 1.0f, 0.0f));
        mat4 lightSpaceMatrix = lightProjectionMatrix * lightViewMatrix;

        // set light position, lightcolor , objectcolor in sphere fragment shader

        SetUniformVec3(shaderScene, "object_color", vec3(1.0f, 1.0f, 1.0f));// white
        SetUniformVec3(shaderScene, "light_color", vec3(1.0, 1.0, 1.0));
        SetUniformVec3(shaderScene, "light_position", lightPosition);

        SetUniformVec3(groundShader, "object_color", vec3(1.0f, 1.0f, 1.0f));// white
        SetUniformVec3(groundShader, "light_color", vec3(1.0, 1.0, 1.0));
        SetUniformVec3(groundShader, "light_position", lightPosition);
        //// Set light space matrix on both shaders
            // set light space matrix on shadow shader
        SetUniformMat4(shaderShadow, "light_view_proj_matrix", lightSpaceMatrix);
        SetUniformMat4(shaderScene, "light_view_proj_matrix", lightSpaceMatrix);
        SetUniformMat4(groundShader, "light_view_proj_matrix", lightSpaceMatrix);

        SetUniformVec3(shaderScene, "view_position", camera.Position);
        SetUniformVec3(groundShader, "view_position", camera.Position);
        //// Set light far and near planes on scene shader

        SetUniform1Value(shaderScene, "light_near_plane", lightNearPlane);
        SetUniform1Value(shaderScene, "light_far_plane", lightFarPlane);

        SetUniform1Value(groundShader, "light_near_plane", lightNearPlane);
        SetUniform1Value(groundShader, "light_far_plane", lightFarPlane);
        // Set light position on scene shader / shpere shader
        //// Set light direction on scene shader
        SetUniformVec3(shaderScene, "light_direction", lightDirection);
        SetUniformVec3(groundShader, "light_direction", lightDirection);
        // set texture ID

        SetUniform1Value(groundShader, "textureSampler", 0);

#pragma endregion set and pass light parameters, shadow, textures to shader
        // set matrix
        mat4 worldRotation = rotate(mat4(1.0f), radians(worldRotateAmount), vec3(worldRotateXaxis, worldRotateYaxis, 0.0f));


        glBindFramebuffer(GL_FRAMEBUFFER, 0);

# pragma endregion
        // render sphere/ as a piano player
        renderSphere(shaderScene, sphereAO);

        //render keyboard
        renderPiano(groundShader, groundAO, whiteKey, blackKey);
        //cout << (translateVariableX) << endl;
        playPiano(); // hit key, play sound, reset key position

        SetUniformMat4(vertex_frag_Shader, "viewMatrix", viewMat);
        SetUniformMat4(shaderScene, "view_matrix", viewMat);
        SetUniformMat4(groundShader, "view_matrix", viewMat);
         
        /// <summary>
        /// Project MoveMent
        /// </summary>
        /// <param name="argc"></param>
        /// <param name="argv"></param>
        /// <returns></returns>
        project_move->Draw(myShader, dt);
        /// <summary>
       /// Color
       /// </summary>
       /// <param name="argc"></param>
       /// <param name="argv"></param>
       /// <returns></returns>
        shaderMyObject.use();
        shaderMyObject.setMat4("projection", projectionMatrix);
        shaderMyObject.setMat4("view", viewMat);
        myObject->Draw(shaderMyObject, dt);
       
        /// <summary>
        /// Main Aera
        /// </summary>
        /// <param name="argc"></param>
        /// <param name="argv"></param>
        /// <returns></returns>
        myShader->use();

        glBindVertexArray(groundVao);

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, grassMap );
        //shaderScene->setInt("isTextured", TOGGLE_TEXTURE);
        glm::mat4 gridModelMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(0, -1.0f,0));
        gridModelMatrix = glm::scale(gridModelMatrix, glm::vec3(200, 1.0f, 200));

        myShader->setMat4("modelMat", gridModelMatrix);
                

        //SetUniformMat4(shaderScene, "model_matrix", gridModelMatrix);
        glDrawArrays(GL_TRIANGLES, 0, 36);
        glBindTexture(GL_TEXTURE_2D, 0);

        
        
        
        
        
        
        
        
        glUniformMatrix4fv(glGetUniformLocation(myShader->ID, "modelMat"), 1, GL_FALSE, glm::value_ptr(modelMat));
        glUniformMatrix4fv(glGetUniformLocation(myShader->ID, "viewMat"), 1, GL_FALSE, glm::value_ptr(viewMat));
        glUniformMatrix4fv(glGetUniformLocation(myShader->ID, "projMat"), 1, GL_FALSE, glm::value_ptr(projMat));

        museum_model.Draw(*myShader);

        if (MODEL_NUM == 1) {
            //manModelMat = glm::translate(glm::mat4(1.0f), glm::vec3(deltX, 0.0f, deltZ));

            //manModelMat = glm::scale(manModelMat, glm::vec3(Scale, Scale, Scale));
            //manModelMat = glm::rotate(manModelMat, glm::radians(rotateAngle), glm::vec3(0.0, 1.0f, 0.0));
        }
            
        glUniformMatrix4fv(glGetUniformLocation(myShader->ID, "modelMat"), 1, GL_FALSE, glm::value_ptr(manModelMat));
        

        glUniform3f(glGetUniformLocation(myShader->ID, "objColor"), 1.0f, 1.0f, 1.0f);
        glUniform3f(glGetUniformLocation(myShader->ID, "ambientColor"), 1.0f, 1.0f, 1.0f);
        glUniform3f(glGetUniformLocation(myShader->ID, "cameraPos"), camera.Position.x, camera.Position.y, camera.Position.z);
            
        glUniform3f(glGetUniformLocation(myShader->ID, "lightD.pos"), lightD.position.x, lightD.position.y, lightD.position.z);
        glUniform3f(glGetUniformLocation(myShader->ID, "lightD.color"), lightD.color.x, lightD.color.y, lightD.color.z);
        glUniform3f(glGetUniformLocation(myShader->ID, "lightD.dirToLight"), lightD.direction.x, lightD.direction.y, lightD.direction.z);

        glUniform3f(glGetUniformLocation(myShader->ID, "lightDFront.pos"), lightDFront.position.x, lightDFront.position.y, lightDFront.position.z);
        glUniform3f(glGetUniformLocation(myShader->ID, "lightDFront.color"), lightDFront.color.x, lightDFront.color.y, lightDFront.color.z);
        glUniform3f(glGetUniformLocation(myShader->ID, "lightDFront.dirToLight"), lightDFront.direction.x, lightDFront.direction.y, lightDFront.direction.z);

        glUniform3f(glGetUniformLocation(myShader->ID, "lightDBack.pos"), lightDBack.position.x, lightDBack.position.y, lightDBack.position.z);
        glUniform3f(glGetUniformLocation(myShader->ID, "lightDBack.color"), lightDBack.color.x, lightDBack.color.y, lightDBack.color.z);
        glUniform3f(glGetUniformLocation(myShader->ID, "lightDBack.dirToLight"), lightDBack.direction.x, lightDBack.direction.y, lightDBack.direction.z);

        glUniform3f(glGetUniformLocation(myShader->ID, "lightDLeft.pos"), lightDLeft.position.x, lightDLeft.position.y, lightDLeft.position.z);
        glUniform3f(glGetUniformLocation(myShader->ID, "lightDLeft.color"), lightDLeft.color.x, lightDLeft.color.y, lightDLeft.color.z);
        glUniform3f(glGetUniformLocation(myShader->ID, "lightDLeft.dirToLight"), lightDLeft.direction.x, lightDLeft.direction.y, lightDLeft.direction.z);

        glUniform3f(glGetUniformLocation(myShader->ID, "lightDRight.pos"), lightDRight.position.x, lightDRight.position.y, lightDRight.position.z);
        glUniform3f(glGetUniformLocation(myShader->ID, "lightDRight.color"), lightDRight.color.x, lightDRight.color.y, lightDRight.color.z);
        glUniform3f(glGetUniformLocation(myShader->ID, "lightDRight.dirToLight"), lightDRight.direction.x, lightDRight.direction.y, lightDRight.direction.z);
            
        glUniform3f(glGetUniformLocation(myShader->ID, "lightP0.pos"), lightP0.position.x, lightP0.position.y, lightP0.position.z);
        glUniform3f(glGetUniformLocation(myShader->ID, "lightP0.color"), lightP0.color.x, lightP0.color.y, lightP0.color.z);
        glUniform3f(glGetUniformLocation(myShader->ID, "lightP0.dirToLight"), lightP0.direction.x, lightP0.direction.y, lightP0.direction.z);
        glUniform1f(glGetUniformLocation(myShader->ID, "lightP0.constant"), lightP0.constant);
        glUniform1f(glGetUniformLocation(myShader->ID, "lightP0.linear"), lightP0.linear);
        glUniform1f(glGetUniformLocation(myShader->ID, "lightP0.quadratic"), lightP0.quadratic);

        myMaterial->shader->setUniform3f("material.ambient", myMaterial->ambient);
        myMaterial->shader->setUniform1f("material.shininess", myMaterial->shininess);
        myMaterial->shader->setUniform1i("material.diffuse", Shader::DIFFUSE);
        myMaterial->shader->setUniform1i("material.specular", Shader::SPECULAR);
        
            
        camera.updateCameraPos();
        //cout << camera.Position.x<< ": "<< camera.Position.y<<": "<< camera.Position.z << endl;
        processInput(window, dt, project_move,myObject, shape);


        glfwSwapBuffers(window);
        glfwPollEvents();
        
        
    }

    glfwTerminate();
    return 0;
} 

void processInput(GLFWwindow* window,float &dt, ProjectMove* project_move, MyObject* myobj, Shape * shape) {
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, true);
    }
    if (glfwGetKey(window, GLFW_KEY_HOME) == GLFW_PRESS)
    {
        if (MODEL_NUM == 2) {     
            project_move->moveZ = 0;
            project_move->moveX = 0;
            project_move->moveY = 0;
            project_move->spinningAngle = 0;
            project_move->spinningAngleY = 0;
            project_move->stopPoint = false;
        }
       
    }
    if (glfwGetKey(window, GLFW_KEY_U) == GLFW_PRESS) {
        Scale += scaleSpeed * dt;
    }

    // j: scale down model
    if (glfwGetKey(window, GLFW_KEY_J) == GLFW_PRESS) {
        Scale -= scaleSpeed * dt;
    }
    if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS) {
        deltX -= 10.0f * dt;

    }
    if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS) {
        deltX += 10.0f * dt;

    }
    if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS) {
        deltZ += 10.0f * dt;

    }
    if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS) {
        deltZ -= 10.0f * dt;

    }
    if (glfwGetKey(window, GLFW_KEY_Z) == GLFW_PRESS) {
        rotateAngle += 20.0f * dt;

    }
    if (glfwGetKey(window, GLFW_KEY_C) == GLFW_PRESS) {
        rotateAngle -= 20.0f * dt;

    }
    if (glfwGetKey(window, GLFW_KEY_4) == GLFW_PRESS) {
        MODEL_NUM = 4;
    }
    if (glfwGetKey(window, GLFW_KEY_3) == GLFW_PRESS) {
        MODEL_NUM = 3;
    }
    if (glfwGetKey(window, GLFW_KEY_2) == GLFW_PRESS) {
        if (MODEL_NUM == 3) {
            myobj->currentModels = 1;
        }
        else {
            MODEL_NUM = 2;
        }
    }
    if (glfwGetKey(window, GLFW_KEY_1) == GLFW_PRESS ) {
        if (MODEL_NUM == 3) {
            myobj->currentModels = 0;
        }
        else {
            MODEL_NUM = 1;
        }
    } 
    if (glfwGetKey(window, GLFW_KEY_0) == GLFW_PRESS) {
        if (MODEL_NUM == 3) {
            myobj->currentModels = -1;
        }
        else {
            MODEL_NUM = 0;
        }
        
    }
    

    if (glfwGetKey(window, GLFW_KEY_L) == GLFW_PRESS) {
        if (MODEL_NUM == 3) {
            myobj->updateObjColor(TEXTURE);
        }
    }
    if (glfwGetKey(window, GLFW_KEY_M) == GLFW_PRESS) {
        if (MODEL_NUM == 3) {
            myobj->updateObjColor(SINGLE);
        }
    }
    if (glfwGetKey(window, GLFW_KEY_N) == GLFW_PRESS) {
        if (MODEL_NUM == 3) {
            myobj->updateObjColor(GRADIENT);
        }       
    }

    // let car spinning
    if (glfwGetKey(window, GLFW_KEY_R) == GLFW_PRESS)
    {
        if (MODEL_NUM == 2) {
            project_move->spinningAngle += 45.0f * dt;
        }
        
    }

    // let car back flip
    if (glfwGetKey(window, GLFW_KEY_B) == GLFW_PRESS)
    {
        if (MODEL_NUM == 2) {
            project_move->spinningAngleY += 90.0f * dt;
            project_move->moveY += 1.0f * dt;
        }
    }
    // let car front flip
    if (glfwGetKey(window, GLFW_KEY_F) == GLFW_PRESS)
    {
        if (MODEL_NUM == 2) {
            project_move->spinningAngleY -= 90.0f * dt;
            project_move->moveY += 1.0f * dt;
        }
        
    }

    if (glfwGetKey(window, GLFW_KEY_Y) == GLFW_PRESS)
    {
        if (MODEL_NUM == 2) {
            if (project_move->stopPoint) {
                project_move->moveZ -= 200.0f * dt * cos(radians(project_move->spinningAngle));
                project_move->moveX -= 200.0f * dt * sin(radians(project_move->spinningAngle));
                project_move->moveY += 1.0f * dt;
            }
            if (project_move->moveY < 2.0f && !project_move->stopPoint)
            {
                project_move->moveY += 2.0f * dt;
            }
            if (project_move->moveY >= 2.0f && !project_move->stopPoint) {
                project_move->moveZ += 5.0f * dt * cos(radians(project_move->spinningAngle));
                project_move->moveX += 5.0f * dt * sin(radians(project_move->spinningAngle));
            }
            if (project_move->moveZ > 2.0f * cos(radians(project_move->spinningAngle)) || project_move->moveX > 2.0f * sin(radians(project_move->spinningAngle))) {
                project_move->stopPoint = true;
            }
        }
       
    }

    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
        if (MODEL_NUM == 1) {
            translateVariableY += 0.05f;
        }
        else if (MODEL_NUM == 2) {
            project_move->moveZ += 5.0f * dt;
        }
        else{
            camera.SpeedZ = 50.0f;
        }
        
    }
    else if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
        if (MODEL_NUM == 1) {
            translateVariableY -= 0.05f;
        }
        else if (MODEL_NUM == 2) {
            project_move->moveZ -= 5.0f * dt;
        }
        else {
            camera.SpeedZ = -50.0f;
        }
    }
    else {
        camera.SpeedZ = 0;
    }

    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
        if (MODEL_NUM == 1) {
            translateVariableX += 0.03f;
        }
        else if (MODEL_NUM == 2) {
            project_move->moveX += 5.0f * dt;
        }
        else {
            camera.SpeedX = 50.0f;
        }
    }
    else if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
        if (MODEL_NUM == 1) {
            translateVariableX -= 0.03f;
        }
        else if (MODEL_NUM == 2) {
            project_move->moveX -= 5.0f * dt;
        }
        else {
            camera.SpeedX = -50.0f;
        }
    }
    else {
        camera.SpeedX = 0;
    }
   

}
