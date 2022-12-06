
//
// COMP 371 Labs Framework
//
// Created by Nicolas Bergeron on 20/06/2019.
//

#include <iostream>
#include <list>

#define GLEW_STATIC 1   // This allows linking with Static Library on Windows, without DLL
#include <GL/glew.h>    // Include GLEW - OpenGL Extension Wrangler
#include <GL/GLU.h>
#include <GL/GL.h>
#include <GLFW/glfw3.h> // cross-platform interface for creating a graphical context,
                        // initializing OpenGL and binding inputs

#include <glm/glm.hpp>  // GLM is an optimized math library with syntax to similar to OpenGL Shading Language
#include <glm/gtc/matrix_transform.hpp> // include this to create transformation matrices
#include <glm/common.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <vector>
#include <cstdlib>
#include <cstring>
#include <stdio.h>
#include <stdlib.h>

#include "Shader.h"


#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"





using namespace glm;
using namespace std;

const int Y_SEGMENTS = 50;
const int X_SEGMENTS = 50;
const int MIN_VALUE = -50;
const int MAX_VALUE = 50;
const float scaleSpeed = 0.5f;
const float moveSpeed = 5.0f;
const GLfloat PI = 3.14159265358979323846f;
GLenum renderMode = GL_TRIANGLES; 
int TOGGLE_TEXTURE = 1;
int TOGGLE_SHADOW = 1;
static void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);
namespace SNOWMAN_PARAM
{
    // Initial Parameter of Snowman
    vec3 temp(0.0f);
    float angle = 0.0f;
    float Scale = 1.0f;
    vec3 base_pos(0.0f);
    vec3 raletive_pos_temp[8] = { vec3(0.0f),vec3(0.0f), vec3(0.0f), vec3(0.0f), vec3(0.0f), vec3(0.0f), vec3(0.0f), vec3(0.0f) };

    float theta = radians(90.0f);
    float phi = 0;
    int index = 0;

    float thrshhold = 0.0f;
    vec3 leftFeetMovement(0.0f);
    vec3 rightFeetMovement(0.0f);
    bool backward = false;

    float angleThrshhold = 0.0f;
    float leftArmAngle = 0.0f;
    float rightArmAngle = 0.0f;
    bool armBackward = false;

    bool KEEP_MOVING_FORWARD = false;
    bool KEEP_MOVING_BACKWARD = false;
}

vec3 snowManVertice[576];
float axisVertices[] = {
    0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, // pos * 3, color * 3
    5.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f,
    0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f,
    0.0f, 5.0f, 0.0f, 0.0f, 1.0f, 0.0f,
    0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f,
    0.0f, 0.0f, 5.0f, 0.0f, 0.0f, 1.0f
};
float unitGroundVertices[] = {
      -50.0f, 0.0f, -50.0f,  0.0f,  25.0f,  0.0f, 1.0f, 0.0f,  // top-left
      50.0f,  0.0f, -50.0f,  25.0f, 25.0f,  0.0f, 1.0f, 0.0f,   // top-right
      50.0f,  0.0f,  50.0f,  25.0f, 0.0f,   0.0f, 1.0f, 0.0f,  // bottom-right
      50.0f,  0.0f,  50.0f,  25.0f, 0.0f,   0.0f, 1.0f, 0.0f,  // bottom-right
      -50.0f, 0.0f,  50.0f,  0.0f,  0.0f,   0.0f, 1.0f, 0.0f, // bottom-left
      -50.0f, 0.0f, -50.0f,  0.0f,  25.0f,  0.0f, 1.0f, 0.0f // top-left
};
float unitLineVertices[] = {
      -1.0f, 0.0f, 0.0f, 1.0f, 1.0f, 0.0f, // pos * 3, color * 3
      1.0f, 0.0f, 0.0f, 1.0f, 1.0f, 0.0f
};

vec3 snowManPos[] = {

    vec3(-1.5f, 0.5f, 0.0f),//0feet1
    vec3(1.5f, 0.5f, 0.0f),//1feet2  
    vec3(0.0f, 7.0f, 0.0f),//2belly
    vec3(0.0f, 13.5f, 0.0f),//3body
    vec3(6.0f, 13.5f, 0.0f),//4arm1
    vec3(-6.0f, 13.5f, 0.0f),//5arm2
    vec3(0.0f, 17.0f, 0.0f),//6head
    vec3(0.0f, 17.0f, 3.0f),//7nose
    vec3(-1.75f, 17.0f, 2.0f),//8eyes1
    vec3(1.75f, 17.0f, 2.0f),//9eyes2
    vec3(-1.75f, 20.0f, 0.0f),//10hair1
    vec3(0.0f, 20.5f, 0.0f),//11hair2
    vec3(1.75f, 20.0f, 0.0f),//12hair3
    vec3(-10.0f, 13.5f, 0.0f),//13finger1
    vec3(10.0f, 13.5f, 0.0f)//14finger2
};

vec3 snowManScale[] = {

    vec3(2.0f, 1.0f, 2.0f),//0
    vec3(2.0f, 1.0f, 2.0f),//1
    vec3(6.0f, 6.0f, 4.0f),//2
    vec3(7.0f, 3.0f, 3.0f),//3
    vec3(7.0f, 1.0f, 1.0f),//4
    vec3(7.0f, 1.0f, 1.0f),//5
    vec3(5.0f, 3.5f, 2.45f),//6
    vec3(1.0f, 1.0f, 1.0f),//7
    vec3(0.75f, 0.75f, 0.75f),//8
    vec3(0.75f, 0.75f, 0.75f),//9
    vec3(0.25f, 4.0f, 0.25f),//10
    vec3(1.0f, 1.0f, 1.0f),//11
    vec3(0.25f, 4.0f, 0.25f),//12
    vec3(1.0f, 0.25f, 0.25f),//13
    vec3(1.0f, 0.25f, 0.25f)//14
};
// window dimensions
const GLuint WIDTH = 1024, HEIGHT = 768;


bool loadOBJ2(
    const char* path,
    std::vector<int>& vertexIndices,
    std::vector<glm::vec3>& temp_vertices,
    std::vector<glm::vec3>& out_normals,
    std::vector<glm::vec2>& out_uvs) {

    std::vector<int> uvIndices, normalIndices;
    std::vector<glm::vec2> temp_uvs;
    std::vector<glm::vec3> temp_normals;

    FILE* file;
    file = fopen(path, "r");
    if (!file) {
        printf("Impossible to open the file ! Are you in the right path ?\n");
        getchar();
        return false;
    }

    while (1) {

        char lineHeader[128];
        // read the first word of the line
        int res = fscanf(file, "%s", lineHeader);
        if (res == EOF)
            break; // EOF = End Of File. Quit the loop.

        // else : parse lineHeader

        if (strcmp(lineHeader, "v") == 0) {
            glm::vec3 vertex;
            res = fscanf(file, "%f %f %f\n", &vertex.x, &vertex.y, &vertex.z);

            temp_vertices.push_back(vertex);
        }
        else if (strcmp(lineHeader, "vt") == 0) {
            glm::vec2 uv;
            res = fscanf(file, "%f %f\n", &uv.x, &uv.y);
            if (res != 2) {
                printf("Missing uv information!\n");
            }
            uv.y = -uv.y; // Invert V coordinate since we will only use DDS texture, which are inverted. Remove if you want to use TGA or BMP loaders.
            temp_uvs.push_back(uv);
        }
        else if (strcmp(lineHeader, "vn") == 0) {
            glm::vec3 normal;
            res = fscanf(file, "%f %f %f\n", &normal.x, &normal.y, &normal.z);
            if (res != 3) {
                printf("Missing normal information!\n");
            }
            temp_normals.push_back(normal);
        }
        else if (strcmp(lineHeader, "f") == 0) {
            char* getRes;
            int vertexIndex[3], uvIndex[3], normalIndex[3];
            bool uv = true;
            bool norm = true;
            char line[128];
            getRes = fgets(line, 128, file);
            if (getRes == 0) {
                printf("incomplete face\n");
            }

            //vertex, uv, norm
            int matches = sscanf(line, "%d/%d/%d %d/%d/%d %d/%d/%d\n", &vertexIndex[0], &uvIndex[0], &normalIndex[0], &vertexIndex[1], &uvIndex[1], &normalIndex[1], &vertexIndex[2], &uvIndex[2], &normalIndex[2]);
            if (matches != 9) {
                //vertex, norm
                matches = sscanf(line, "%d//%d %d//%d %d//%d\n", &vertexIndex[0], &normalIndex[0], &vertexIndex[1], &normalIndex[1], &vertexIndex[2], &normalIndex[2]);
                if (matches != 6) {
                    //vertex, uv
                    matches = sscanf(line, "%d/%d %d/%d %d/%d\n", &vertexIndex[0], &uvIndex[0], &vertexIndex[1], &uvIndex[1], &vertexIndex[2], &uvIndex[2]);
                    if (matches != 6) {
                        //vertex
                        matches = sscanf(line, "%d %d %d\n", &vertexIndex[0], &vertexIndex[1], &vertexIndex[2]);
                        if (matches != 3) {
                            printf("File can't be read by our simple parser. 'f' format expected: d/d/d d/d/d d/d/d || d/d d/d d/d || d//d d//d d//d\n");
                            printf("Character at %ld", ftell(file));
                            return false;
                        }
                        uv, norm = false;
                    }
                    else {
                        norm = false;
                    }
                }
                else {
                    uv = false;
                }
            }
            vertexIndices.push_back(abs(vertexIndex[0]) - 1);
            vertexIndices.push_back(abs(vertexIndex[1]) - 1);
            vertexIndices.push_back(abs(vertexIndex[2]) - 1);
            if (norm) {
                normalIndices.push_back(abs(normalIndex[0]) - 1);
                normalIndices.push_back(abs(normalIndex[1]) - 1);
                normalIndices.push_back(abs(normalIndex[2]) - 1);
            }
            if (uv) {
                uvIndices.push_back(abs(uvIndex[0]) - 1);
                uvIndices.push_back(abs(uvIndex[1]) - 1);
                uvIndices.push_back(abs(uvIndex[2]) - 1);
            }
        }
        else {
            char clear[1000];
            char* getsRes = fgets(clear, 1000, file);
        }
    }
    if (normalIndices.size() != 0)
        out_normals.resize(temp_normals.size());
    if (uvIndices.size() != 0)
        out_uvs.resize(temp_normals.size());
    for (unsigned int i = 0; i < vertexIndices.size(); i++) {
        int vi = vertexIndices[i];
        if (normalIndices.size() != 0) {
            int ni = normalIndices[i];
            out_normals[vi] = temp_normals[ni];
        }
        if (uvIndices.size() != 0 && i < uvIndices.size()) {
            int ui = uvIndices[i];
            out_uvs[vi] = temp_uvs[ui];
        }
    }

    return true;
}
GLuint setupModelEBO(string path, int& vertexCount)
{
    vector<int> vertexIndices;
    //The contiguous sets of three indices of vertices, normals and UVs, used to make a triangle
    vector<glm::vec3> vertices;
    vector<glm::vec3> normals;
    vector<glm::vec2> UVs;

    //read the vertices from the cube.obj file
    //We won't be needing the normals or UVs for this program
    loadOBJ2(path.c_str(), vertexIndices, vertices, normals, UVs);

    GLuint VAO;
    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO); //Becomes active VAO
    // Bind the Vertex Array Object first, then bind and set vertex buffer(s) and attribute pointer(s).

    //Vertex VBO setup
    GLuint vertices_VBO;
    glGenBuffers(1, &vertices_VBO);
    glBindBuffer(GL_ARRAY_BUFFER, vertices_VBO);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(glm::vec3), &vertices.front(), GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid*)0);
    glEnableVertexAttribArray(0);

    //Normals VBO setup
    GLuint normals_VBO;
    glGenBuffers(1, &normals_VBO);
    glBindBuffer(GL_ARRAY_BUFFER, normals_VBO);
    glBufferData(GL_ARRAY_BUFFER, normals.size() * sizeof(glm::vec3), &normals.front(), GL_STATIC_DRAW);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid*)0);
    glEnableVertexAttribArray(1);

    //UVs VBO setup
    GLuint uvs_VBO;
    glGenBuffers(1, &uvs_VBO);
    glBindBuffer(GL_ARRAY_BUFFER, uvs_VBO);
    glBufferData(GL_ARRAY_BUFFER, UVs.size() * sizeof(glm::vec2), &UVs.front(), GL_STATIC_DRAW);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(GLfloat), (GLvoid*)0);
    glEnableVertexAttribArray(2);

    //EBO setup
    GLuint EBO;
    glGenBuffers(1, &EBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, vertexIndices.size() * sizeof(int), &vertexIndices.front(), GL_STATIC_DRAW);

    glBindVertexArray(0);
    // Unbind VAO (it's always a good thing to unbind any buffer/array to prevent strange bugs), remember: do NOT unbind the EBO, keep it bound to this VAO
    vertexCount = vertexIndices.size();
    return VAO;
}


GLFWwindow* window = nullptr;
bool InitContext();
struct TexturedColoredVertex
{
    TexturedColoredVertex(vec3 _position, vec3 _color, vec2 _uv)
        : position(_position), color(_color), uv(_uv) {}

    vec3 position;
    vec3 color;
    vec2 uv;
};

// Textured Cube model
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

unsigned int loadImageToGPU(const char* filename, GLint internalformat, GLenum format, int textureSlot)
{
    stbi_set_flip_vertically_on_load(true);
    unsigned int TexBuffer;
    glGenTextures(1, &TexBuffer);
    //glActiveTexture(GL_TEXTURE0 + textureSlot);
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
    if (!InitContext()) return -1;

    glfwSetKeyCallback(window, keyCallback);
    // Black background
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

    //std::string shaderPathPrefix = "assets/shaders/";
    Shader* shaderScene = new Shader("scene_vertex.glsl", "scene_fragment.glsl");
    Shader* shaderShadow = new Shader("shadow_vertex.glsl", "shadow_fragment.glsl");


    std::string exePath = argv[0];
    int hatVertices;
    GLuint hatVAO = setupModelEBO(exePath.substr(0, exePath.find_last_of('\\')) + "\\" + "\\model_hat\\hat.obj", hatVertices);

    int activeHatVertices = hatVertices;
    GLuint activeHatVAO = hatVAO;

    int vao = createTexturedCubeVertexArrayObject();
    unsigned int axisVao, axisVbo;

    // bind axis data
    glGenVertexArrays(1, &axisVao);
    glGenBuffers(1, &axisVbo);
    glBindVertexArray(axisVao);
    glBindBuffer(GL_ARRAY_BUFFER, axisVbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(axisVertices), axisVertices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), nullptr);
    glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(3);
    //
    //sphere
    std::vector<float> sphereVertices;
    std::vector<int> sphereIndices;
    float nx, ny, nz, lengthInv = 1.0f / 1.0f;
    /*2-计算球体顶点*/
    //生成球的顶点
    for (int y = 0; y <= Y_SEGMENTS; y++)
    {
        for (int x = 0; x <= X_SEGMENTS; x++)
        {

            float xSegment = (float)x / (float)X_SEGMENTS;
            float ySegment = (float)y / (float)Y_SEGMENTS;
            float xPos = std::cos(xSegment * 2.0f * PI) * std::sin(ySegment * PI);
            float yPos = std::cos(ySegment * PI);
            float zPos = std::sin(xSegment * 2.0f * PI) * std::sin(ySegment * PI);
            nx = xPos * lengthInv;
            ny = yPos * lengthInv;
            nz = zPos * lengthInv;
            sphereVertices.push_back(xPos);
            sphereVertices.push_back(yPos);
            sphereVertices.push_back(zPos);
            sphereVertices.push_back(nx);;
            sphereVertices.push_back(ny);
            sphereVertices.push_back(nz);
        }
    }

    //生成球的Indices
    for (int i = 0; i < Y_SEGMENTS; i++)
    {
        for (int j = 0; j < X_SEGMENTS; j++)
        {
            sphereIndices.push_back(i * (X_SEGMENTS + 1) + j);
            sphereIndices.push_back((i + 1) * (X_SEGMENTS + 1) + j);
            sphereIndices.push_back((i + 1) * (X_SEGMENTS + 1) + j + 1);
            sphereIndices.push_back(i * (X_SEGMENTS + 1) + j);
            sphereIndices.push_back((i + 1) * (X_SEGMENTS + 1) + j + 1);
            sphereIndices.push_back(i * (X_SEGMENTS + 1) + j + 1);
        }
    }


    /*3-数据处理*/
    unsigned int ballVBO, ballVAO;
    glGenVertexArrays(1, &ballVAO);
    glGenBuffers(1, &ballVBO);
    //生成并绑定球体的VAO和VBO
    glBindVertexArray(ballVAO);
    glBindBuffer(GL_ARRAY_BUFFER, ballVBO);
    //将顶点数据绑定至当前默认的缓冲中
    glBufferData(GL_ARRAY_BUFFER, sphereVertices.size() * sizeof(float), &sphereVertices[0], GL_STATIC_DRAW);

    GLuint element_buffer_object;//EBO
    glGenBuffers(1, &element_buffer_object);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, element_buffer_object);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sphereIndices.size() * sizeof(int), &sphereIndices[0], GL_STATIC_DRAW);

    //设置顶点属性指针
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    //解绑VAO和VBO
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
    // Setup texture and framebuffer for creating shadow map

    // Dimensions of the shadow texture, which should cover the viewport window size and shouldn't be oversized and waste resources
    const unsigned int DEPTH_MAP_TEXTURE_SIZE = 1024;

    // Variable storing index to texture used for shadow mapping
    GLuint depth_map_texture;
    // Get the texture
    glGenTextures(1, &depth_map_texture);
    // Bind the texture so the next glTex calls affect it
    glBindTexture(GL_TEXTURE_2D, depth_map_texture);
    // Create the texture and specify it's attributes, including widthn height, components (only depth is stored, no color information)
    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, DEPTH_MAP_TEXTURE_SIZE, DEPTH_MAP_TEXTURE_SIZE, 0, GL_DEPTH_COMPONENT, GL_FLOAT,
        NULL);
    // Set texture sampler parameters.
    // The two calls below tell the texture sampler inside the shader how to upsample and downsample the texture. Here we choose the nearest filtering option, which means we just use the value of the closest pixel to the chosen image coordinate.
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    // The two calls below tell the texture sampler inside the shader how it should deal with texture coordinates outside of the [0, 1] range. Here we decide to just tile the image.
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);


    // Variable storing index to framebuffer used for shadow mapping
    GLuint depth_map_fbo;  // fbo: framebuffer object
    // Get the framebuffer
    glGenFramebuffers(1, &depth_map_fbo);
    // Bind the framebuffer so the next glFramebuffer calls affect it
    glBindFramebuffer(GL_FRAMEBUFFER, depth_map_fbo);
    // Attach the depth map texture to the depth map framebuffer
    //glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, depth_map_texture, 0);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depth_map_texture, 0);
    glDrawBuffer(GL_NONE); //disable rendering colors, only write depth values


    //texture
    unsigned int gridMap;
    gridMap = loadTexture("snowtexture.jpg");
    std::cout << "gridMap :" << gridMap << std::endl;
    unsigned int noseMap;
    noseMap = loadTexture("carrot.jpg");
    std::cout << "noseMap :" << noseMap << std::endl;
    unsigned int eyeMap;
    eyeMap = loadTexture("eye.jpg");
    std::cout << "eyeMap :" << eyeMap << std::endl;
    unsigned int hatMap;
    hatMap = loadTexture("pink_metal.jpg");
    std::cout << "hatMap :" << hatMap << std::endl;
    unsigned int armMap;
    armMap = loadTexture("branch.jpg");
    std::cout << "armMap :" << armMap << std::endl;

  // Camera parameters for view transform
    vec3 cameraPosition(0.0f, 20.0f, 35.0f);
    vec3 cameraLookAt(0.0f, 0.0f, -1.0f);
    vec3 cameraUp(0.0f, 1.0f, 0.0f);

    // Other camera parameters
    float cameraSpeed = 1.0f;
    float cameraFastSpeed = 3 * cameraSpeed;
    float cameraHorizontalAngle = 90.0f;
    float cameraVerticalAngle = 0.0f;

    // For spinning model
    float spinningAngle = 0.0f;

    // Set projection matrix for shader, this won't change
    mat4 projectionMatrix = glm::perspective(70.0f,           // field of view in degrees
        WIDTH * 1.0f / HEIGHT, // aspect ratio
        0.01f, 800.0f);  // near and far (near > 0)

// Set initial view matrix on both shaders
    mat4 viewMatrix = lookAt(cameraPosition,                // eye
        cameraPosition + cameraLookAt, // center
        cameraUp);                     // up

// Set projection matrix on both shaders
    shaderScene->setMat4("projection_matrix", projectionMatrix);
    //SetUniformMat4(shaderScene, "projection_matrix", projectionMatrix);

    // Set view matrix on both shaders
    shaderScene->setMat4("view_matrix", viewMatrix);
    //SetUniformMat4(shaderScene, "view_matrix", viewMatrix);


    float lightAngleOuter = 30.0;
    float lightAngleInner = 20.0;
    // Set light cutoff angles on scene shader
    shaderScene->setFloat("light_cutoff_inner", cos(radians(lightAngleInner)));
    shaderScene->setFloat("light_cutoff_outer", cos(radians(lightAngleOuter)));
    //SetUniform1Value(shaderScene, "light_cutoff_inner", cos(radians(lightAngleInner)));
    //SetUniform1Value(shaderScene, "light_cutoff_outer", cos(radians(lightAngleOuter)));

    // Set light color on scene shader
    shaderScene->setVec3("light_color", vec3(1.0, 1.0, 1.0));
    //SetUniformVec3(shaderScene, "light_color", vec3(1.0, 1.0, 1.0));

    // Set object color on scene shader
    shaderScene->setVec3("object_color", vec3(1.0, 1.0, 1.0));
    //SetUniformVec3(shaderScene, "object_color", vec3(1.0, 1.0, 1.0));

    // For frame time
    float lastFrameTime = glfwGetTime();
    int lastMouseLeftState = GLFW_RELEASE;
    double lastMousePosX, lastMousePosY;
    glfwGetCursorPos(window, &lastMousePosX, &lastMousePosY);

    // Other OpenGL states to set once
    // Enable Backface culling
    glEnable(GL_DEPTH_TEST);
    //glEnable(GL_CULL_FACE);
    // Entering Main Loop
    while (!glfwWindowShouldClose(window)) {
        // Frame time calculation
        float dt = glfwGetTime() - lastFrameTime;
        lastFrameTime = glfwGetTime();


        // light parameters
        vec3 lightPosition = vec3(10.0f, 50.0f, 50.0f); // the location of the light in 3D space
            //vec3(sinf(glfwGetTime() * 6.0f * 3.141592f), sinf(glfwGetTime() * 3.141592f), cosf(glfwGetTime() * 3.141592f));
        vec3 lightFocus(0.0, 0.0, 0.0);      // the point in 3D space the light "looks" at
        vec3 lightDirection = normalize(lightFocus - lightPosition);

        float lightNearPlane = 20.0f;
        float lightFarPlane = 180.0f;

        mat4 lightProjectionMatrix = //frustum(-1.0f, 1.0f, -1.0f, 1.0f, lightNearPlane, lightFarPlane);
        perspective(glm::radians(85.0f), (float)DEPTH_MAP_TEXTURE_SIZE / (float)DEPTH_MAP_TEXTURE_SIZE, lightNearPlane, lightFarPlane);
        mat4 lightViewMatrix = lookAt(lightPosition, lightFocus, vec3(0.0f, 1.0f, 0.0f));
        mat4 lightSpaceMatrix = lightProjectionMatrix * lightViewMatrix;

        // Set light space matrix on both shaders
        shaderShadow->setMat4("light_view_proj_matrix", lightSpaceMatrix);
        shaderScene->setMat4("light_view_proj_matrix", lightSpaceMatrix);
        //SetUniformMat4(shaderShadow, "light_view_proj_matrix", lightSpaceMatrix);
        //SetUniformMat4(shaderScene, "light_view_proj_matrix", lightSpaceMatrix);

        // Set light far and near planes on scene shader
        shaderScene->setFloat("light_near_plane", lightNearPlane);
        shaderScene->setFloat("light_far_plane", lightFarPlane);
        //SetUniform1Value(shaderScene, "light_near_plane", lightNearPlane);
        //SetUniform1Value(shaderScene, "light_far_plane", lightFarPlane);

        // Set light position on scene shader
        shaderScene->setVec3("light_position", lightPosition);
        //SetUniformVec3(shaderScene, "light_position", lightPosition);

        // Set light direction on scene shader
        shaderScene->setVec3("light_direction", lightDirection);
        //SetUniformVec3(shaderScene, "light_direction", lightDirection);

        // Spinning model rotation animation
       

        // Set model matrix and send to both shaders
        mat4 modelMatrix = mat4(1.0f);
            //glm::translate(mat4(1.0f), vec3(0.0f, 1.0f, -3.0f)) *
            //glm::rotate(mat4(1.0f), radians(spinningAngle), vec3(0.0f, 1.0f, 0.0f)) *
            //glm::rotate(mat4(1.0f), radians(-90.0f), vec3(1.0f, 0.0f, 0.0f)) *
            //glm::scale(mat4(1.0f), vec3(0.1f));

        shaderScene->setMat4("model_matrix", modelMatrix);
        shaderShadow->setMat4("model_matrix", modelMatrix);
        //SetUniformMat4(shaderScene, "model_matrix", modelMatrix);
        //SetUniformMat4(shaderShadow, "model_matrix", modelMatrix);

        // Set the view matrix for first person camera and send to both shaders
        mat4 viewMatrix = lookAt(cameraPosition, cameraPosition + cameraLookAt, cameraUp);
        shaderScene->setMat4("view_matrix", viewMatrix);

        // Set view position on scene shader
        shaderScene->setVec3("view_position", cameraPosition);
        //SetUniformVec3(shaderScene, "view_position", cameraPosition);

        // Render shadow in 2 passes: 1- Render depth map, 2- Render scene
        // 1- Render shadow map:
        // a- use program for shadows
        // b- resize window coordinates to fix depth map output size
        // c- bind depth map framebuffer to output the depth values
        {
            // Use proper shader
            //glUseProgram(shaderShadow);
            shaderShadow->use();
            shaderShadow->setInt("isShadowed", TOGGLE_SHADOW);
            // Use proper image output size
            glViewport(0, 0, DEPTH_MAP_TEXTURE_SIZE, DEPTH_MAP_TEXTURE_SIZE);
            // Bind depth map texture as output framebuffer
            glBindFramebuffer(GL_FRAMEBUFFER, depth_map_fbo);
            // Clear depth data on the framebuffer
            glClear(GL_DEPTH_BUFFER_BIT);
            // Bind geometry
            //glBindVertexArray(activeVAO);
            // Draw geometry
            //glDrawElements(GL_TRIANGLES, activeVertices, GL_UNSIGNED_INT, 0);
            glBindVertexArray(vao);
            mat4 gridModelMatrix = translate(glm::mat4(1.0f),vec3(0, 2.0f,0));
            /*shaderShadow->setMat4("model_matrix", gridModelMatrix);
            glDrawArrays(GL_TRIANGLES, 0, 36);*/
            //draw ground
            for (int i = -50; i < 50; i++) {
                for (int j = -50; j < 50; j++)
                {
                   gridModelMatrix = translate(glm::mat4(1.0f), glm::vec3((float)i, -1.0f, (float)j));
                    shaderShadow->setMat4("model_matrix", gridModelMatrix);
                    //SetUniformMat4(shaderShadow, "model_matrix", gridModelMatrix);
                    glDrawArrays(GL_TRIANGLES, 0, 36);
                }

            }
            //Draw snowman
            for (size_t i = 0; i < 15; i++)
            {
                SNOWMAN_PARAM::index = i;

                if (i == 3) {
                    glBindVertexArray(ballVAO);
                    glDrawElements(GL_TRIANGLES, X_SEGMENTS * Y_SEGMENTS * 6, GL_UNSIGNED_INT, 0);

                }
                else if (i == 7) {
                    glBindVertexArray(ballVAO);
                    glDrawElements(GL_TRIANGLES, X_SEGMENTS * Y_SEGMENTS * 6, GL_UNSIGNED_INT, 0);
                }if (i == 12) {

                    // Bind geometry
                    glBindVertexArray(activeHatVAO);
                    // Draw geometry
                    glDrawElements(GL_TRIANGLES, activeHatVertices, GL_UNSIGNED_INT, 0);

                }
                 


                glBindVertexArray(vao);
                modelMatrix = translate(mat4(1.0f), SNOWMAN_PARAM::temp);

                modelMatrix = scale(modelMatrix, glm::vec3(SNOWMAN_PARAM::Scale, SNOWMAN_PARAM::Scale, SNOWMAN_PARAM::Scale));
                if (i == 4 || i == 14)
                {
                    modelMatrix = rotate(modelMatrix, glm::radians(SNOWMAN_PARAM::leftArmAngle), normalize(vec3(0, 1, 0)));
                }
                else if (i == 5 || i == 13)
                {
                    modelMatrix = rotate(modelMatrix, glm::radians(SNOWMAN_PARAM::rightArmAngle), normalize(vec3(0, 1, 0)));
                }
                modelMatrix = rotate(modelMatrix, glm::radians(SNOWMAN_PARAM::angle), normalize(vec3(0, 1, 0)));

                modelMatrix = translate(modelMatrix, (+snowManPos[i]));

                modelMatrix = scale(modelMatrix, snowManScale[i]);

                if (i == 0) {
                    modelMatrix = translate(modelMatrix, SNOWMAN_PARAM::leftFeetMovement);
                }
                else if (i == 1)
                {
                    modelMatrix = translate(modelMatrix, SNOWMAN_PARAM::rightFeetMovement);
                }
                shaderShadow->setInt("index", i);
                //glUniform1i(glGetUniformLocation(myShader->ID, "index"), i);
                shaderShadow->setMat4("model_matrix", modelMatrix);
                //glUniformMatrix4fv(gridModelLocation, 1, GL_FALSE, glm::value_ptr(modelMat));


                glDrawArrays(renderMode, 0, 36);


            }
            
            // Unbind geometry
            glBindVertexArray(0);
        }


        //2- Render scene: a- bind the default framebuffer and b- just render like what we do normally
        {
            // Use proper shader
            shaderScene->use();
            shaderScene->setInt("isShadowed", TOGGLE_SHADOW);
            //glUseProgram(shaderScene);
            // Use proper image output size
            // Side note: we get the size from the framebuffer instead of using WIDTH and HEIGHT because of a bug with highDPI displays
            int width, height;
            glfwGetFramebufferSize(window, &width, &height);
            glViewport(0, 0, width, height);
            // Bind screen as output framebuffer
            glBindFramebuffer(GL_FRAMEBUFFER, 0);
            // Clear color and depth data on framebuffer
            glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
            // Bind depth map texture: not needed, by default it is active
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, depth_map_texture);
            //draw axis
            
            glBindVertexArray(axisVao);
            glLineWidth(5.0f);
            shaderScene->setMat4("model_matrix", mat4(1.0f));
            shaderScene->setInt("index", 7);
            glDrawArrays(GL_LINES, 0, 2);
            glDrawArrays(GL_LINES, 2, 2);
            glDrawArrays(GL_LINES, 4, 2);
            glLineWidth(1.0f);

            glBindVertexArray(vao);
            mat4 gridModelMatrix = translate(glm::mat4(1.0f), vec3(0, 2.0f, 0));
            /*shaderScene->setMat4("model_matrix", gridModelMatrix);
            glDrawArrays(GL_TRIANGLES, 0, 36);*/
            //draw ground
            
            for (int i = -50; i < 50; i++) {
                for (int j = -50; j < 50; j++)
                {
                    glActiveTexture(GL_TEXTURE1);
                    glBindTexture(GL_TEXTURE_2D, gridMap);

                    shaderScene->setInt("toggleGround", 1);
                    shaderScene->setInt("gridMap", 1);
                    shaderScene->setInt("isTextured", TOGGLE_TEXTURE);
                    
                    //shaderScene->setInt("isTextured", TOGGLE_TEXTURE);
                    glm::mat4 gridModelMatrix = glm::translate(glm::mat4(1.0f), glm::vec3((float)i, -1.0f, (float)j));
                    shaderScene->setMat4("model_matrix", gridModelMatrix);
                    //SetUniformMat4(shaderScene, "model_matrix", gridModelMatrix);
                    glDrawArrays(GL_TRIANGLES, 0, 36);
                }

            }
            
            //Draw snowman
            for (size_t i = 0; i < 15; i++)
            {
                SNOWMAN_PARAM::index = i;
                shaderScene->setInt("isTextured", 0);
                if (i == 0 || i == 1) {
                    glActiveTexture(GL_TEXTURE1);
                    glBindTexture(GL_TEXTURE_2D, armMap);
                    shaderScene->setInt("toggleArm", 1);
                    //shaderScene->setInt("armMap", 1);
                    //myShader->setUniform1i("noseMap", 1);
                    shaderScene->setInt("isTextured", TOGGLE_TEXTURE);
                }
                else if (i == 3) {
                    glBindVertexArray(ballVAO);
                    glDrawElements(GL_TRIANGLES, X_SEGMENTS * Y_SEGMENTS * 6, GL_UNSIGNED_INT, 0);

                }
                else if (i == 7) {
                    glBindVertexArray(ballVAO);
                    glDrawElements(GL_TRIANGLES, X_SEGMENTS * Y_SEGMENTS * 6, GL_UNSIGNED_INT, 0);
                    glActiveTexture(GL_TEXTURE1);
                    glBindTexture(GL_TEXTURE_2D, noseMap);
                    shaderScene->setInt("toggleNose", 1);
                    shaderScene->setInt("isTextured", TOGGLE_TEXTURE);
                    //shaderScene->setInt("noseMap",1);
                }
                else if (i == 8 || i == 9) {
                    glActiveTexture(GL_TEXTURE1);
                    glBindTexture(GL_TEXTURE_2D, eyeMap);
                    shaderScene->setInt("toggleEye", 1);
                    shaderScene->setInt("isTextured", TOGGLE_TEXTURE);
                    //myShader->setUniform1i("eyeMap", 2);
                    //shaderScene->setInt("eyeMap", 1);
                }
                else if (i == 10)
                {
                    glActiveTexture(GL_TEXTURE1);
                    glBindTexture(GL_TEXTURE_2D, hatMap);
                    shaderScene->setInt("toggleHat", 1);
                    shaderScene->setInt("isTextured", TOGGLE_TEXTURE);
                    //shaderScene->setInt("hatMap", 1);
                }
                else if (i == 12) {
                    glActiveTexture(GL_TEXTURE1);
                    glBindTexture(GL_TEXTURE_2D, hatMap);
                    shaderScene->setInt("toggleHat", 1);
                    shaderScene->setInt("isTextured", TOGGLE_TEXTURE);
                    // Bind geometry
                    glBindVertexArray(activeHatVAO);
                    // Draw geometry
                    glDrawElements(GL_TRIANGLES, activeHatVertices, GL_UNSIGNED_INT, 0);
                    

                }

                
                glBindVertexArray(vao);
                modelMatrix = translate(mat4(1.0f), SNOWMAN_PARAM::temp);

                modelMatrix = scale(modelMatrix, glm::vec3(SNOWMAN_PARAM::Scale, SNOWMAN_PARAM::Scale, SNOWMAN_PARAM::Scale));
                if (i == 4 || i == 14)
                {
                    modelMatrix = rotate(modelMatrix, glm::radians(SNOWMAN_PARAM::leftArmAngle), normalize(vec3(0, 1, 0)));
                }
                else if (i == 5 || i == 13)
                {
                    modelMatrix = rotate(modelMatrix, glm::radians(SNOWMAN_PARAM::rightArmAngle), normalize(vec3(0, 1, 0)));
                }
                modelMatrix = rotate(modelMatrix, glm::radians(SNOWMAN_PARAM::angle), normalize(vec3(0, 1, 0)));

                modelMatrix = translate(modelMatrix, (+snowManPos[i]));

                modelMatrix = scale(modelMatrix, snowManScale[i]);

                if (i == 0) {
                    modelMatrix = translate(modelMatrix, SNOWMAN_PARAM::leftFeetMovement);
                }
                else if (i == 1)
                {
                    modelMatrix = translate(modelMatrix, SNOWMAN_PARAM::rightFeetMovement);
                }
                shaderScene->setInt("index", i);
                //glUniform1i(glGetUniformLocation(myShader->ID, "index"), i);
                shaderScene->setMat4("model_matrix", modelMatrix);
                //glUniformMatrix4fv(gridModelLocation, 1, GL_FALSE, glm::value_ptr(modelMat));


                glDrawArrays(renderMode, 0, 36);


            }
            // Unbind geometry
            glBindVertexArray(0);
        }
        /**/

        glfwSwapBuffers(window);
        glfwPollEvents();

#pragma region Camera Update
        // Everything below here is the solution for Lab02 - Moving camera exercise
        // We'll change this to be a first or third person camera
        bool fastCam = glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS || glfwGetKey(window, GLFW_KEY_RIGHT_SHIFT) ==
            GLFW_PRESS;
        float currentCameraSpeed = (fastCam) ? cameraFastSpeed : cameraSpeed;


        // - Calculate mouse motion dx and dy
        // - Update camera horizontal and vertical angle
        double mousePosX, mousePosY;
        glfwGetCursorPos(window, &mousePosX, &mousePosY);

        double dx = mousePosX - lastMousePosX;
        double dy = mousePosY - lastMousePosY;

        lastMousePosX = mousePosX;
        lastMousePosY = mousePosY;

        // Convert to spherical coordinates
        const float cameraAngularSpeed = 60.0f;
        cameraHorizontalAngle -= dx * cameraAngularSpeed * dt;
        cameraVerticalAngle -= dy * cameraAngularSpeed * dt;

        // Clamp vertical angle to [-85, 85] degrees
        cameraVerticalAngle = std::max(-85.0f, std::min(85.0f, cameraVerticalAngle));
        if (cameraHorizontalAngle > 360)
        {
            cameraHorizontalAngle -= 360;
        }
        else if (cameraHorizontalAngle < -360)
        {
            cameraHorizontalAngle += 360;
        }

       

        cameraLookAt = vec3(cosf(SNOWMAN_PARAM::phi) * cosf(SNOWMAN_PARAM::theta), sinf(SNOWMAN_PARAM::phi), -cosf(SNOWMAN_PARAM::phi) * sinf(SNOWMAN_PARAM::theta));
        vec3 cameraSideVector = glm::cross(cameraLookAt, vec3(0.0f, 1.0f, 0.0f));

        vec3 panVector = glm::cross(cameraLookAt, vec3(0.0f, 1.0f, 0.0f));
        glm::normalize(panVector);

        vec3 tiltVector = glm::cross(cameraLookAt, vec3(1.0f, 0.0f, 0.0f));
        glm::normalize(tiltVector);

        viewMatrix = lookAt(cameraPosition, cameraPosition + cameraLookAt, cameraUp);
        glm::normalize(cameraSideVector);

#pragma endregion

#pragma region escape space P L T U J
        // Handle inputs
        if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
            glfwSetWindowShouldClose(window, true);
        if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS) {
            float randX = rand() % (MAX_VALUE - MIN_VALUE + 1) + MIN_VALUE;
            SNOWMAN_PARAM::temp.x = randX;
            float randZ = rand() % (MAX_VALUE - MIN_VALUE + 1) + MIN_VALUE;
            SNOWMAN_PARAM::temp.z = randZ;
        }

        if (glfwGetKey(window, GLFW_KEY_P) == GLFW_PRESS) // render point
        {
            renderMode = GL_POINTS;
        }

        if (glfwGetKey(window, GLFW_KEY_L) == GLFW_PRESS) // render line
        {
            renderMode = GL_LINES;
        }

        if (glfwGetKey(window, GLFW_KEY_T) == GLFW_PRESS) // render triangle
        {
            renderMode = GL_TRIANGLES;
        }

        if (glfwGetKey(window, GLFW_KEY_U) == GLFW_PRESS) // Scale UP
        {
            SNOWMAN_PARAM::Scale += scaleSpeed * dt;
        }

        if (glfwGetKey(window, GLFW_KEY_J) == GLFW_PRESS) // Scale UP
        {
            SNOWMAN_PARAM::Scale -= scaleSpeed * dt;
        }
#pragma endregion

#pragma region UP DOWN LEFT RIGHT
        if (glfwGetKey(window, GLFW_KEY_HOME) == GLFW_PRESS) // Scale Down
        {
            cameraPosition = vec3(0.6f, 15.0f, 15.0f);
            SNOWMAN_PARAM::theta = radians(90.0f);
            SNOWMAN_PARAM::phi = 0;
        }

        if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS) // Scale Down
        {

            SNOWMAN_PARAM::phi += radians(20.0f) * dt;
        }

        if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS) // Scale Down
        {
            SNOWMAN_PARAM::phi -= radians(20.0f) * dt;
        }

        if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS) // Scale Down
        {
            SNOWMAN_PARAM::theta += radians(20.0f) * dt;
        }

        if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS) // Scale Down
        {
            SNOWMAN_PARAM::theta -= radians(20.0f) * dt;
        }

#pragma endregion

#pragma region 1 2 WASD QEZC

        if (glfwGetKey(window, GLFW_KEY_1) == GLFW_PRESS) // move camera to the left
        {
           
            SNOWMAN_PARAM::angle += 60.0f * dt;
            SNOWMAN_PARAM::leftFeetMovement = vec3(0.0);
            SNOWMAN_PARAM::rightFeetMovement = vec3(0.0);
           
        }
        if (glfwGetKey(window, GLFW_KEY_2) == GLFW_PRESS) // move camera to the left
        {

            SNOWMAN_PARAM::angle -= 60.0f * dt;
            SNOWMAN_PARAM::leftFeetMovement = vec3(0.0);
            SNOWMAN_PARAM::rightFeetMovement = vec3(0.0);

        }

        if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) // move camera to the left
        {
            if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS) {
                SNOWMAN_PARAM::temp -= vec3(0.1f, 0.0f, 0.0f);
                /*if (SNOWMAN_PARAM::backward) {
                    SNOWMAN_PARAM::leftFeetMovement -= vec3(0.0f, 0.0f, 0.1f);
                    SNOWMAN_PARAM::rightFeetMovement += vec3(0.0f, 0.0f, 0.1f);
                    SNOWMAN_PARAM::thrshhold -= 0.1f;
                    if (SNOWMAN_PARAM::thrshhold < -0.5f) {
                        SNOWMAN_PARAM::backward = false;
                    }
                }
                else {
                    SNOWMAN_PARAM::leftFeetMovement += vec3(0.0f, 0.0f, 0.1f);
                    SNOWMAN_PARAM::rightFeetMovement -= vec3(0.0f, 0.0f, 0.1f);
                    SNOWMAN_PARAM::thrshhold += 0.1f;
                    if (SNOWMAN_PARAM::thrshhold > 0.5f) {
                        SNOWMAN_PARAM::backward = true;
                    }
                }*/
            }
            else {
                SNOWMAN_PARAM::angle += 30.0f * dt;
                SNOWMAN_PARAM::leftFeetMovement = vec3(0.0);
                SNOWMAN_PARAM::rightFeetMovement = vec3(0.0);
            }
        }

        if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) // move camera to the right
        {
            if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS) {
                SNOWMAN_PARAM::temp += vec3(0.1f, 0.0f, 0.0f);
                /*if (SNOWMAN_PARAM::backward) {
                    SNOWMAN_PARAM::leftFeetMovement += vec3(0.0f, 0.0f, 0.1f);
                    SNOWMAN_PARAM::rightFeetMovement -= vec3(0.0f, 0.0f, 0.1f);
                    SNOWMAN_PARAM::thrshhold += 0.1f;
                    if (SNOWMAN_PARAM::thrshhold > +0.5f) {
                        SNOWMAN_PARAM::backward = false;
                    }
                }
                else {
                    SNOWMAN_PARAM::leftFeetMovement -= vec3(0.0f, 0.0f, 0.1f);
                    SNOWMAN_PARAM::rightFeetMovement += vec3(0.0f, 0.0f, 0.1f);
                    SNOWMAN_PARAM::thrshhold -= 0.1f;
                    if (SNOWMAN_PARAM::thrshhold < -0.5f) {
                        SNOWMAN_PARAM::backward = true;
                    }
                }*/
            }
            else {
                SNOWMAN_PARAM::angle -= 30.0f * dt;
                SNOWMAN_PARAM::leftFeetMovement = vec3(0.0);
                SNOWMAN_PARAM::rightFeetMovement = vec3(0.0);
            }

        }
        if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) // move camera up
        {
            SNOWMAN_PARAM::temp += vec3(0.0f, 0.0f, 0.1f);

        }

        if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) // move camera down
        {
            SNOWMAN_PARAM::temp -= vec3(0.0f, 0.0f, 0.1f);

        }
        if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS || SNOWMAN_PARAM::KEEP_MOVING_FORWARD) {
            SNOWMAN_PARAM::temp += vec3(0.0f, 0.0f, 0.1f);
            //feet
            if (SNOWMAN_PARAM::backward) {
                SNOWMAN_PARAM::leftFeetMovement -= vec3(0.0f, 0.0f, 0.1f);
                SNOWMAN_PARAM::rightFeetMovement += vec3(0.0f, 0.0f, 0.1f);
                SNOWMAN_PARAM::thrshhold -= 0.1f;
                if (SNOWMAN_PARAM::thrshhold < -0.5f) {
                    SNOWMAN_PARAM::backward = false;
                }
            }
            else {
                SNOWMAN_PARAM::leftFeetMovement += vec3(0.0f, 0.0f, 0.1f);
                SNOWMAN_PARAM::rightFeetMovement -= vec3(0.0f, 0.0f, 0.1f);
                SNOWMAN_PARAM::thrshhold += 0.1f;
                if (SNOWMAN_PARAM::thrshhold > 0.5f) {
                    SNOWMAN_PARAM::backward = true;
                }
            }
            //hand
            if (SNOWMAN_PARAM::armBackward) {
                SNOWMAN_PARAM::leftArmAngle -= 30.0f * dt;
                SNOWMAN_PARAM::rightArmAngle += 30.0f * dt;
                SNOWMAN_PARAM::angleThrshhold -= 5.0f;
                if (SNOWMAN_PARAM::angleThrshhold < -20.0f) {
                    SNOWMAN_PARAM::armBackward = false;
                }
            }
            else {
                SNOWMAN_PARAM::leftArmAngle += 30.0f * dt;
                SNOWMAN_PARAM::rightArmAngle -= 30.0f * dt;
                SNOWMAN_PARAM::angleThrshhold += 5.0f;
                if (SNOWMAN_PARAM::angleThrshhold > 20.0f) {
                    SNOWMAN_PARAM::armBackward = true;
                }
            }

        }


        if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS || SNOWMAN_PARAM::KEEP_MOVING_BACKWARD) {
            SNOWMAN_PARAM::temp -= vec3(0.0f, 0.0f, 0.1f);
            //feet
            if (SNOWMAN_PARAM::backward) {
                SNOWMAN_PARAM::leftFeetMovement += vec3(0.0f, 0.0f, 0.1f);
                SNOWMAN_PARAM::rightFeetMovement -= vec3(0.0f, 0.0f, 0.1f);
                SNOWMAN_PARAM::thrshhold += 0.1f;
                if (SNOWMAN_PARAM::thrshhold > +0.5f) {
                    SNOWMAN_PARAM::backward = false;
                }
            }
            else {
                SNOWMAN_PARAM::leftFeetMovement -= vec3(0.0f, 0.0f, 0.1f);
                SNOWMAN_PARAM::rightFeetMovement += vec3(0.0f, 0.0f, 0.1f);
                SNOWMAN_PARAM::thrshhold -= 0.1f;
                if (SNOWMAN_PARAM::thrshhold < -0.5f) {
                    SNOWMAN_PARAM::backward = true;
                }
            }
            //hand
            if (SNOWMAN_PARAM::armBackward) {
                SNOWMAN_PARAM::leftArmAngle -= 30.0f * dt;
                SNOWMAN_PARAM::rightArmAngle += 30.0f * dt;
                SNOWMAN_PARAM::angleThrshhold -= 5.0f;
                if (SNOWMAN_PARAM::angleThrshhold < -20.0f) {
                    SNOWMAN_PARAM::armBackward = false;
                }
            }
            else {
                SNOWMAN_PARAM::leftArmAngle += 30.0f * dt;
                SNOWMAN_PARAM::rightArmAngle -= 30.0f * dt;
                SNOWMAN_PARAM::angleThrshhold += 5.0f;
                if (SNOWMAN_PARAM::angleThrshhold > 20.0f) {
                    SNOWMAN_PARAM::armBackward = true;
                }
            }

        }

#pragma endregion

#pragma region PAN TILE
        //pan
        if (dx < 0) {
            if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_PRESS) {
                cameraPosition += panVector * currentCameraSpeed ;
            }
        }
        else if (dx > 0) {
            if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_PRESS) {
                cameraPosition -= panVector * currentCameraSpeed ;
            }
        }
        //tilt
        if (dy < 0) {
            if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_MIDDLE) == GLFW_PRESS) {
                cameraPosition += tiltVector * currentCameraSpeed ;
            }

        }
        else if (dy > 0) {
            if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_MIDDLE) == GLFW_PRESS) {
                cameraPosition -= tiltVector * currentCameraSpeed ;
            }
        }
#pragma endregion

#pragma region ZOOM IN OUT
        if (dy < 0) {
            if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS) {

                cameraPosition += cameraLookAt * currentCameraSpeed ;

            }
        }
        else if (dy > 0) {
            if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS) {

                cameraPosition -= cameraLookAt * currentCameraSpeed ;

            }
        }
#pragma endregion 

    }

    glfwTerminate();

    return 0;
}





bool InitContext()
{
    // Initialize GLFW and OpenGL version
    glfwInit();

    //#if defined(PLATFORM_OSX)
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    //#else
      // On windows, we set OpenGL version to 2.1, to support more hardware
     // glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 2);
      //glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
    //#endif

      // Create Window and rendering context using GLFW, resolution is 800x600
    window = glfwCreateWindow(WIDTH, HEIGHT, "Comp371 - Assignment2", NULL, NULL);
    if (window == NULL) {
        std::cerr << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return false;
    }
    //glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    glfwMakeContextCurrent(window);


    // Initialize GLEW
    glewExperimental = true; // Needed for core profile
    if (glewInit() != GLEW_OK) {
        std::cerr << "Failed to create GLEW" << std::endl;
        glfwTerminate();
        return false;
    }

    return true;
}

static void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods) {


    // x: toggle texture
    if (key == GLFW_KEY_X && action == GLFW_PRESS) {
        if (TOGGLE_TEXTURE) {
            TOGGLE_TEXTURE = 0;
        }
        else {
            TOGGLE_TEXTURE = 1;
        }
    }
    if (key == GLFW_KEY_B && action == GLFW_PRESS) {
        if (TOGGLE_SHADOW) {
            TOGGLE_SHADOW = 0;
        }
        else {
            TOGGLE_SHADOW = 1;
        }
    }

    if (glfwGetKey(window, GLFW_KEY_Z) == GLFW_PRESS) {
        if (SNOWMAN_PARAM::KEEP_MOVING_FORWARD) {
            SNOWMAN_PARAM::KEEP_MOVING_FORWARD = false;
        }
        else {
            SNOWMAN_PARAM::KEEP_MOVING_FORWARD = true;
        }

    }
    if (glfwGetKey(window, GLFW_KEY_C) == GLFW_PRESS) {
        if (SNOWMAN_PARAM::KEEP_MOVING_BACKWARD) {
            SNOWMAN_PARAM::KEEP_MOVING_BACKWARD = false;
        }
        else {
            SNOWMAN_PARAM::KEEP_MOVING_BACKWARD = true;
        }

    }
}
