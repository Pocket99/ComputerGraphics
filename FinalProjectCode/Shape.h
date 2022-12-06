#pragma once
#include <iostream>
#include <algorithm>
#include <vector>
#include <list>
#include <fstream>
#include <sstream>

#define GLEW_STATIC 1   // This allows linking with Static Library on Windows, without DLL
#include <GL/glew.h>    // Include GLEW - OpenGL Extension Wrangler

#include <GLFW/glfw3.h> // GLFW provides a cross-platform interface for creating a graphical context,
// initializing OpenGL and binding inputs

#include <glm/glm.hpp>  // GLM is an optimized math library with syntax to similar to OpenGL Shading Language
#include <glm/gtc/matrix_transform.hpp> // include this to create transformation matrices
#include <glm/gtc/type_ptr.hpp>
#include "stb_image.h"
#include "Camera.h"
const int Y_SEGMENTS = 50;
const int X_SEGMENTS = 50;
const GLfloat PI = 3.14159265358979323846f;

using namespace glm;
using namespace std;
class Shape
{
public:  
    // window dimensions
    const GLuint WIDTH = 1024, HEIGHT = 768;
    bool loadOBJ(
        const char* path,
        std::vector<glm::vec3>& out_vertices,
        std::vector<glm::vec3>& out_normals,
        std::vector<glm::vec2>& out_uvs) {

        std::vector<int> vertexIndices, uvIndices, normalIndices;
        std::vector<glm::vec3> temp_vertices;
        std::vector<glm::vec2> temp_uvs;
        std::vector<glm::vec3> temp_normals;

        FILE* file;
        file = fopen(path, "r");
        if (!file) {
            printf("Impossible to open the file ! Are you in the right path ?\n");
            printf(path);
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
                fscanf(file, "%f %f %f\n", &vertex.x, &vertex.y, &vertex.z);
                temp_vertices.push_back(vertex);
            }
            else if (strcmp(lineHeader, "vt") == 0) {
                glm::vec2 uv;
                fscanf(file, "%f %f\n", &uv.x, &uv.y);
                uv.y = -uv.y; // Invert V coordinate since we will only use DDS texture, which are inverted. Remove if you want to use TGA or BMP loaders.
                temp_uvs.push_back(uv);
            }
            else if (strcmp(lineHeader, "vn") == 0) {
                glm::vec3 normal;
                fscanf(file, "%f %f %f\n", &normal.x, &normal.y, &normal.z);
                temp_normals.push_back(normal);
            }
            else if (strcmp(lineHeader, "f") == 0) {
                int vertexIndex[3], uvIndex[3], normalIndex[3];
                bool uv = true;
                bool norm = true;
                char line[128];
                fgets(line, 128, file);
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
                            if (matches != 6) {
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
                vertexIndices.push_back(vertexIndex[0]);
                vertexIndices.push_back(vertexIndex[1]);
                vertexIndices.push_back(vertexIndex[2]);
                if (norm) {
                    normalIndices.push_back(normalIndex[0]);
                    normalIndices.push_back(normalIndex[1]);
                    normalIndices.push_back(normalIndex[2]);
                }
                if (uv) {
                    uvIndices.push_back(uvIndex[0]);
                    uvIndices.push_back(uvIndex[1]);
                    uvIndices.push_back(uvIndex[2]);
                }
            }
            else {
                char clear[1000];
                fgets(clear, 1000, file);
            }

        }
        //std::cout << "Vertex indices: " << vertexIndices.size() << std::endl;
        //std::cout << "UV indices: " << uvIndices.size() << std::endl;
        //std::cout << "Normal indices: " << normalIndices.size() << std::endl;
        // For each vertex of each triangle
        for (unsigned int i = 0; i < vertexIndices.size(); i++) {
            if (uvIndices.size() != 0) {
                if (i < uvIndices.size()) {
                    unsigned int uvIndex = abs(uvIndices[i]);
                    glm::vec2 uv = temp_uvs[uvIndex - 1];
                    out_uvs.push_back(uv);
                }
            }
            if (normalIndices.size() != 0) {
                if (i < normalIndices.size()) {
                    unsigned int normalIndex = abs(normalIndices[i]);
                    glm::vec3 normal = temp_normals[normalIndex - 1];
                    out_normals.push_back(normal);
                }
            }

            unsigned int vertexIndex = abs(vertexIndices[i]);
            glm::vec3 vertex = temp_vertices[vertexIndex - 1];
            out_vertices.push_back(vertex);
        }

        return true;
    }
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

    int loadSHADER(string vertex_file_path, string fragment_file_path) {

        // Create the shaders
        GLuint VertexShaderID = glCreateShader(GL_VERTEX_SHADER);
        GLuint FragmentShaderID = glCreateShader(GL_FRAGMENT_SHADER);

        // Read the Vertex Shader code from the file
        std::string VertexShaderCode;
        std::ifstream VertexShaderStream(vertex_file_path, std::ios::in);
        if (VertexShaderStream.is_open()) {
            std::stringstream sstr;
            sstr << VertexShaderStream.rdbuf();
            VertexShaderCode = sstr.str();
            VertexShaderStream.close();
        }
        else {
            printf("Impossible to open %s. Are you in the right directory ? Don't forget to read the FAQ !\n", vertex_file_path);
            getchar();
            return 0;
        }

        // Read the Fragment Shader code from the file
        std::string FragmentShaderCode;
        std::ifstream FragmentShaderStream(fragment_file_path, std::ios::in);
        if (FragmentShaderStream.is_open()) {
            std::stringstream sstr;
            sstr << FragmentShaderStream.rdbuf();
            FragmentShaderCode = sstr.str();
            FragmentShaderStream.close();
        }

        GLint Result = GL_FALSE;
        int InfoLogLength;

        // Compile Vertex Shader
        cout << "Compiling shader : " << vertex_file_path << endl;
        char const* VertexSourcePointer = VertexShaderCode.c_str();
        glShaderSource(VertexShaderID, 1, &VertexSourcePointer, NULL);
        glCompileShader(VertexShaderID);

        // Check Vertex Shader
        glGetShaderiv(VertexShaderID, GL_COMPILE_STATUS, &Result);
        glGetShaderiv(VertexShaderID, GL_INFO_LOG_LENGTH, &InfoLogLength);
        if (InfoLogLength > 0) {
            std::vector<char> VertexShaderErrorMessage(InfoLogLength + 1);
            glGetShaderInfoLog(VertexShaderID, InfoLogLength, NULL, &VertexShaderErrorMessage[0]);
            printf("%s\n", &VertexShaderErrorMessage[0]);
        }

        // Compile Fragment Shader
        cout << "Compiling shader : " << fragment_file_path << endl;
        char const* FragmentSourcePointer = FragmentShaderCode.c_str();
        glShaderSource(FragmentShaderID, 1, &FragmentSourcePointer, NULL);
        glCompileShader(FragmentShaderID);

        // Check Fragment Shader
        glGetShaderiv(FragmentShaderID, GL_COMPILE_STATUS, &Result);
        glGetShaderiv(FragmentShaderID, GL_INFO_LOG_LENGTH, &InfoLogLength);
        if (InfoLogLength > 0) {
            std::vector<char> FragmentShaderErrorMessage(InfoLogLength + 1);
            glGetShaderInfoLog(FragmentShaderID, InfoLogLength, NULL, &FragmentShaderErrorMessage[0]);
            printf("%s\n", &FragmentShaderErrorMessage[0]);
        }

        // Link the program
        printf("Linking program\n");
        GLuint ProgramID = glCreateProgram();
        glAttachShader(ProgramID, VertexShaderID);
        glAttachShader(ProgramID, FragmentShaderID);
        glLinkProgram(ProgramID);

        // Check the program
        glGetProgramiv(ProgramID, GL_LINK_STATUS, &Result);
        glGetProgramiv(ProgramID, GL_INFO_LOG_LENGTH, &InfoLogLength);
        if (InfoLogLength > 0) {
            std::vector<char> ProgramErrorMessage(InfoLogLength + 1);
            glGetProgramInfoLog(ProgramID, InfoLogLength, NULL, &ProgramErrorMessage[0]);
            printf("%s\n", &ProgramErrorMessage[0]);
        }

        glDetachShader(ProgramID, VertexShaderID);
        glDetachShader(ProgramID, FragmentShaderID);

        glDeleteShader(VertexShaderID);
        glDeleteShader(FragmentShaderID);

        return ProgramID;
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



    // shader variable setters
    void SetUniformMat4(GLuint shader_id, const char* uniform_name, mat4 uniform_value)
    {
        glUseProgram(shader_id);
        glUniformMatrix4fv(glGetUniformLocation(shader_id, uniform_name), 1, GL_FALSE, &uniform_value[0][0]);
    }

    void SetUniformVec3(GLuint shader_id, const char* uniform_name, vec3 uniform_value)
    {
        glUseProgram(shader_id);
        glUniform3fv(glGetUniformLocation(shader_id, uniform_name), 1, value_ptr(uniform_value));
    }

    template <class T>
    void SetUniform1Value(GLuint shader_id, const char* uniform_name, T uniform_value)
    {
        glUseProgram(shader_id);
        glUniform1i(glGetUniformLocation(shader_id, uniform_name), uniform_value);
        glUseProgram(0);
    }
    GLFWwindow* window = nullptr;
    bool InitContext();

    // the following code use to rander sphere, it's from lab05 
    // first build a vertex struct
    struct Vertex
    {
        glm::vec3 position;
        glm::vec3 normal;
        glm::vec3 color;
    };

    int numOfVertices;


    struct posNormUv
    {
        posNormUv(vec3 _position, vec3 _norm, vec3 _color, vec2 _uv)
            : position(_position), norm(_norm), color(_color), uv(_uv) {}

        vec3 position;
        vec3 norm;
        vec3 color;
        vec2 uv;
    };

    int createTexturedCubeWithNormalVertexArrayObject()
    {
        posNormUv posNormUvCubeArray[] = {// position, normals, color, uv
        // back face
         posNormUv(vec3(-1.0f, -1.0f, -1.0f),   vec3(0.0f,  0.0f, -1.0f), vec3(1.0f, 1.0f, 1.0f),      vec2(0.0f, 0.0f)), // bottom-left
         posNormUv(vec3(1.0f,  1.0f, -1.0f),    vec3(0.0f,  0.0f, -1.0f), vec3(1.0f, 1.0f, 1.0f),      vec2(1.0f, 1.0f)), // top-right
         posNormUv(vec3(1.0f, -1.0f, -1.0f),    vec3(0.0f,  0.0f, -1.0f), vec3(1.0f, 1.0f, 1.0f),      vec2(1.0f, 0.0f)), // bottom-right         
         posNormUv(vec3(1.0f,  1.0f, -1.0f),    vec3(0.0f,  0.0f, -1.0f), vec3(1.0f, 1.0f, 1.0f),       vec2(1.0f, 1.0f)), // top-right
         posNormUv(vec3(-1.0f, -1.0f, -1.0f),   vec3(0.0f,  0.0f, -1.0f), vec3(1.0f, 1.0f, 1.0f),       vec2(0.0f, 0.0f)), // bottom-left
         posNormUv(vec3(-1.0f,  1.0f, -1.0f),   vec3(0.0f,  0.0f, -1.0f), vec3(1.0f, 1.0f, 1.0f),       vec2(0.0f, 1.0f)), // top-left
          // front face
         posNormUv(vec3(-1.0f, -1.0f,  1.0f),   vec3(0.0f,  0.0f,  1.0f), vec3(1.0f, 1.0f, 1.0f),        vec2(0.0f, 0.0f)), // bottom-left
         posNormUv(vec3(1.0f, -1.0f,  1.0f),    vec3(0.0f,  0.0f,  1.0f), vec3(1.0f, 1.0f, 1.0f),        vec2(1.0f, 0.0f)), // bottom-right
         posNormUv(vec3(1.0f,  1.0f,  1.0f),    vec3(0.0f,  0.0f,  1.0f), vec3(1.0f, 1.0f, 1.0f),        vec2(1.0f, 1.0f)), // top-right
         posNormUv(vec3(1.0f,  1.0f,  1.0f),    vec3(0.0f,  0.0f,  1.0f), vec3(1.0f, 1.0f, 1.0f),        vec2(1.0f, 1.0f)), // top-right
         posNormUv(vec3(-1.0f,  1.0f,  1.0f),   vec3(0.0f,  0.0f,  1.0f), vec3(1.0f, 1.0f, 1.0f),        vec2(0.0f, 1.0f)), // top-left
         posNormUv(vec3(-1.0f, -1.0f,  1.0f),   vec3(0.0f,  0.0f,  1.0f), vec3(1.0f, 1.0f, 1.0f),        vec2(0.0f, 0.0f)), // bottom-left
          // left face
         posNormUv(vec3(-1.0f,  1.0f,  1.0f),   vec3(-1.0f,  0.0f,  0.0f),vec3(1.0f, 1.0f, 1.0f),       vec2(1.0f, 0.0f)), // top-right
         posNormUv(vec3(-1.0f,  1.0f, -1.0f),   vec3(-1.0f,  0.0f,  0.0f),vec3(1.0f, 1.0f, 1.0f),       vec2(1.0f, 1.0f)), // top-left
         posNormUv(vec3(-1.0f, -1.0f, -1.0f),   vec3(-1.0f,  0.0f,  0.0f),vec3(1.0f, 1.0f, 1.0f),       vec2(0.0f, 1.0f)), // bottom-left
         posNormUv(vec3(-1.0f, -1.0f, -1.0f),   vec3(-1.0f,  0.0f,  0.0f),vec3(1.0f, 1.0f, 1.0f),       vec2(0.0f, 1.0f)), // bottom-left
         posNormUv(vec3(-1.0f, -1.0f,  1.0f),   vec3(-1.0f,  0.0f,  0.0f),vec3(1.0f, 1.0f, 1.0f),       vec2(0.0f, 0.0f)), // bottom-right
         posNormUv(vec3(-1.0f,  1.0f,  1.0f),   vec3(-1.0f,  0.0f,  0.0f),vec3(1.0f, 1.0f, 1.0f),       vec2(1.0f, 0.0f)), // top-right
          // right face
           posNormUv(vec3(1.0f,  1.0f,  1.0f),  vec3(1.0f,  0.0f,  0.0f),vec3(1.0f, 1.0f, 1.0f),        vec2(1.0f, 0.0f)), // top-left
           posNormUv(vec3(1.0f, -1.0f, -1.0f),  vec3(1.0f,  0.0f,  0.0f),vec3(1.0f, 1.0f, 1.0f),        vec2(0.0f, 1.0f)), // bottom-right
           posNormUv(vec3(1.0f,  1.0f, -1.0f),  vec3(1.0f,  0.0f,  0.0f),vec3(1.0f, 1.0f, 1.0f),        vec2(1.0f, 1.0f)), // top-right         
           posNormUv(vec3(1.0f, -1.0f, -1.0f),  vec3(1.0f,  0.0f,  0.0f),vec3(1.0f, 1.0f, 1.0f),        vec2(0.0f, 1.0f)), // bottom-right
           posNormUv(vec3(1.0f,  1.0f,  1.0f),  vec3(1.0f,  0.0f,  0.0f),vec3(1.0f, 1.0f, 1.0f),        vec2(1.0f, 0.0f)), // top-left
           posNormUv(vec3(1.0f, -1.0f,  1.0f),  vec3(1.0f,  0.0f,  0.0f),vec3(1.0f, 1.0f, 1.0f),        vec2(0.0f, 0.0f)), // bottom-left     
          // bottom face
          posNormUv(vec3(-1.0f, -1.0f, -1.0f),  vec3(0.0f, -1.0f,  0.0f),vec3(1.0f, 1.0f, 1.0f),        vec2(0.0f, 1.0f)), // top-right
          posNormUv(vec3(1.0f, -1.0f,  -1.0f),  vec3(0.0f, -1.0f,  0.0f),vec3(1.0f, 1.0f, 1.0f),        vec2(1.0f, 1.0f)), // top-left
          posNormUv(vec3(1.0f, -1.0f,   1.0f),  vec3(0.0f, -1.0f,  0.0f),vec3(1.0f, 1.0f, 1.0f),        vec2(1.0f, 0.0f)), // bottom-left
          posNormUv(vec3(1.0f, -1.0f,   1.0f),  vec3(0.0f, -1.0f,  0.0f),vec3(1.0f, 1.0f, 1.0f),        vec2(1.0f, 0.0f)), // bottom-left
          posNormUv(vec3(-1.0f, -1.0f,  1.0f),  vec3(0.0f, -1.0f,  0.0f),vec3(1.0f, 1.0f, 1.0f),        vec2(0.0f, 0.0f)), // bottom-right
          posNormUv(vec3(-1.0f, -1.0f, -1.0f),  vec3(0.0f, -1.0f,  0.0f),vec3(1.0f, 1.0f, 1.0f),        vec2(0.0f, 1.0f)), // top-right
          // top face
          posNormUv(vec3(-1.0f,  1.0f, -1.0f),  vec3(0.0f,  1.0f,  0.0f), vec3(1.0f, 1.0f, 1.0f),       vec2(0.0f, 1.0f)), // top-left
          posNormUv(vec3(1.0f,  1.0f , 1.0f),   vec3(0.0f,  1.0f,  0.0f), vec3(1.0f, 1.0f, 1.0f),       vec2(1.0f, 0.0f)), // bottom-right
          posNormUv(vec3(1.0f,  1.0f, -1.0f),   vec3(0.0f,  1.0f,  0.0f), vec3(1.0f, 1.0f, 1.0f),       vec2(1.0f, 1.0f)), // top-right     
          posNormUv(vec3(1.0f,  1.0f,  1.0f),   vec3(0.0f,  1.0f,  0.0f), vec3(1.0f, 1.0f, 1.0f),       vec2(1.0f, 0.0f)), // bottom-right
          posNormUv(vec3(-1.0f,  1.0f, -1.0f),  vec3(0.0f,  1.0f,  0.0f), vec3(1.0f, 1.0f, 1.0f),       vec2(0.0f, 1.0f)), // top-left
          posNormUv(vec3(-1.0f,  1.0f,  1.0f),  vec3(0.0f,  1.0f,  0.0f), vec3(1.0f, 1.0f, 1.0f),       vec2(0.0f, 0.0f))  // bottom-left        
        };
        // Create a vertex array
        GLuint vertexArrayObject;
        glGenVertexArrays(1, &vertexArrayObject);
        glBindVertexArray(vertexArrayObject);

        // Upload Vertex Buffer to the GPU, keep a reference to it (vertexBufferObject)
        GLuint vertexBufferObject;
        glGenBuffers(1, &vertexBufferObject);
        glBindBuffer(GL_ARRAY_BUFFER, vertexBufferObject);
        glBufferData(GL_ARRAY_BUFFER, sizeof(posNormUvCubeArray), posNormUvCubeArray, GL_STATIC_DRAW);

        glVertexAttribPointer(0,                   // attribute 0 matches aPos in Vertex Shader
            3,                   // size
            GL_FLOAT,            // type
            GL_FALSE,            // normalized?
            sizeof(posNormUv), // stride - each vertex contain 2 vec3 (position, color)
            (void*)0             // array buffer offset
        );
        glEnableVertexAttribArray(0);


        glVertexAttribPointer(1,                            // attribute 1 matches normals in Vertex Shader
            3,
            GL_FLOAT,
            GL_FALSE,
            sizeof(posNormUv),
            (void*)sizeof(glm::vec3)      // color is offseted a vec3 (comes after position)
        );
        glEnableVertexAttribArray(1);

        glVertexAttribPointer(2,                            // attribute 1 matches color
            3,
            GL_FLOAT,
            GL_FALSE,
            sizeof(posNormUv),
            (void*)(2 * sizeof(glm::vec3))      // color is offseted two vec3 (comes after position and normals)
        );
        glEnableVertexAttribArray(2);

        //// 3rd attribute buffer : UV
        glVertexAttribPointer(3,
            2,
            GL_FLOAT,
            GL_FALSE,
            sizeof(posNormUv),
            (void*)(3 * sizeof(glm::vec3)) // uv is Offseted by 3 vec3 
        );
        glEnableVertexAttribArray(3);

        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindVertexArray(0);

        return vertexArrayObject;
    }

    // bubble class
    class Bubble
    {
    public:
        Bubble(vec3 position, vec3 velocity, int shaderProgram) : mPosition(position), mVelocity(velocity)
        {
            mWorldMatrixLocation = glGetUniformLocation(shaderProgram, "worldMatrix");

        }

        void Update(float dt)
        {
            mPosition += vec3(0.0f, 1.0f, 0.0f) * dt;
        }

        void Draw() {
            // this is a bit of a shortcut, since we have a single vbo, it is already bound
            // let's just set the world matrix in the vertex shader
            mat4 worldMatrix = translate(mat4(1), vec3(-33.0f, 0.0f, 10.0f));
            worldMatrix = translate(worldMatrix, mPosition) * rotate(mat4(1.0f), radians(180.0f), vec3(0.0f, 1.0f, 0.0f)) * scale(mat4(1.0f), vec3(0.7f, 0.7f, 0.7f));
            glUniformMatrix4fv(mWorldMatrixLocation, 1, GL_FALSE, &worldMatrix[0][0]);
            glDrawElements(GL_TRIANGLES, X_SEGMENTS * Y_SEGMENTS * 6, GL_UNSIGNED_INT, 0);
        }

    private:
        GLuint mWorldMatrixLocation;
        vec3 mPosition;
        vec3 mVelocity;
    };

    GLuint groundTextureID;
    GLuint carrotTextureID;
    GLuint woodTextureID;
    GLuint skyTextureID;
    GLuint xTextureID;
    GLuint blackTextureID ;
    GLuint silverTextureID;
    GLuint zTextureID;
    std::string shaderPathPrefix = "assets/shaders/";
    GLuint shaderShadow;
    GLuint shaderSphere;
    GLuint shaderTextured;
    float lightButton = 1.0f;
    list<Bubble> bubbleList;
    // this varible sue to toggle texture on/off
    bool toggleButton = true;
    const unsigned int DEPTH_MAP_TEXTURE_SIZE = 1024;
    // world parameters
    float worldInitRotateAngleX = 0.0f;
    float worldInitRotateAngleY = 0.0f;
    float worldInitRotateAngleZ = 0.0f;

    // cube parameters
    float initRotateAngle = 0.0f;
    vec3 bubbleMachineScale = vec3(2.0f, 2.0f, 2.0f);


    bool shadowAlgorithm = true;

    // Spinning cube at camera position
    float spinningCubeAngle = 0.0f;

    // spinning model
    float spinningAngle = 0.0f;

    unsigned int ballVBO, ballVAO;

    //Setup models
    string ayaPath;

    int ayaVertices;
    GLuint ayaVAO;

    int activeVertices;
    GLuint activeVAO;
    GLuint depth_map_texture; 
    GLuint depth_map_fbo;
    int texturedVbO;
    int lastKeyQState;
    Shape(Camera camera) {
    
        // Load Textures
        groundTextureID = loadTexture("assets/textures/ground.jpg");
        carrotTextureID = loadTexture("assets/textures/carrot.jpg");
        woodTextureID = loadTexture("assets/textures/wood.jpg");
        skyTextureID = loadTexture("assets/textures/sky.jpg");
        xTextureID = loadTexture("assets/textures/x.jpg");
        blackTextureID = loadTexture("assets/textures/black.jpg");
        silverTextureID = loadTexture("assets/textures/silver.jpg");
        zTextureID = loadTexture("assets/textures/z.jpg");


        // Compile and link shaders here ...
        //int shaderProgram = compileAndLinkShaders();
        shaderPathPrefix = "assets/shaders/";
        shaderShadow = loadSHADER(shaderPathPrefix + "shadow_vertex.glsl", shaderPathPrefix + "shadow_fragment.glsl");
        shaderSphere = loadSHADER(shaderPathPrefix + "sphvertexshader.glsl", shaderPathPrefix + "sphfrashader.glsl");
        shaderTextured = loadSHADER(shaderPathPrefix + "TexturedVertexShader.glsl", shaderPathPrefix + "TexturedFragmentShader.glsl");

        //Setup models
        ayaPath = "assets/models/091_W_Aya_10k.obj";


        ayaVAO = setupModelEBO(ayaPath, ayaVertices);

        activeVertices = ayaVertices;
        activeVAO = ayaVAO;

        texturedVbO = createTexturedCubeWithNormalVertexArrayObject();

        
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
         // fbo: framebuffer object
        // Get the framebuffer
        glGenFramebuffers(1, &depth_map_fbo);
        // Bind the framebuffer so the next glFramebuffer calls affect it
        glBindFramebuffer(GL_FRAMEBUFFER, depth_map_fbo);
        // Attach the depth map texture to the depth map framebuffer
        // glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, depth_map_texture, 0);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depth_map_texture, 0);
        glDrawBuffer(GL_NONE); // disable rendering colors, only write depth values
        //glBindFramebuffer(GL_FRAMEBUFFER, 0);

        

        // Set projection matrix for shader, this won't change
        mat4 projectionMatrix = glm::perspective(70.0f,           // field of view in degrees
            1024.0f / 768.0f, // aspect ratio
            0.01f, 400.0f);  // near and far (near > 0)

        // Set projection matrix on both shaders
        SetUniformMat4(shaderTextured, "projectionMatrix", projectionMatrix);
        SetUniformMat4(shaderSphere, "projectionMatrix", projectionMatrix);

        // Set initial view matrix
        mat4 viewMatrix = lookAt(camera.Position,                // eye
            camera.Position + camera.Forward, // center
            camera.WorldUp);                     // up

        // Set view matrix on both shaders
        SetUniformMat4(shaderTextured, "viewMatrix", viewMatrix);
        SetUniformMat4(shaderSphere, "viewMatrix", viewMatrix);

        float lightAngleOuter = 30.0;
        float lightAngleInner = 20.0;
        // Set light cutoff angles on scene shader
        SetUniform1Value(shaderTextured, "light_cutoff_inner", cos(radians(lightAngleInner)));
        SetUniform1Value(shaderTextured, "light_cutoff_outer", cos(radians(lightAngleOuter)));
        SetUniform1Value(shaderSphere, "light_cutoff_inner", cos(radians(lightAngleInner)));
        SetUniform1Value(shaderSphere, "light_cutoff_outer", cos(radians(lightAngleOuter)));

        // Set light color on scene shader
        SetUniformVec3(shaderTextured, "light_color", vec3(1.0, 1.0, 1.0));
        SetUniformVec3(shaderSphere, "light_color", vec3(1.0, 1.0, 1.0));

        // Define and upload geometry to the GPU here ...
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
        int texturedVbO = createTexturedCubeWithNormalVertexArrayObject();
        lastKeyQState = GLFW_RELEASE;





    }

    void Draw(Camera camera , float dt, mat4 viewMatrix, GLFWwindow* window){
        // light parameters
        vec3 lightPosition = vec3(0.0f, 0.0f, 30.0f); // the location of the light in 3D space
        //vec3(sinf(glfwGetTime() * 6.0f * 3.141592f), sinf(glfwGetTime() * 3.141592f), cosf(glfwGetTime() * 3.141592f));
        vec3 lightFocus(0.0, 0.0, -1.0);      // the point in 3D space the light "looks" at
        vec3 lightDirection = normalize(lightFocus - lightPosition);

        float lightNearPlane = 0.001f;
        float lightFarPlane = 400.0f;

        mat4 lightProjectionMatrix = frustum(-1.0f, 1.0f, -1.0f, 1.0f, lightNearPlane, lightFarPlane);
        //perspective(20.0f, (float)DEPTH_MAP_TEXTURE_SIZE / (float)DEPTH_MAP_TEXTURE_SIZE, lightNearPlane, lightFarPlane);
        mat4 lightViewMatrix = lookAt(lightPosition, lightFocus, vec3(0.0f, 1.0f, 0.0f));
        mat4 lightSpaceMatrix = lightProjectionMatrix * lightViewMatrix * lightButton;

        SetUniformVec3(shaderTextured, "light_position", lightPosition);
        SetUniform1Value(shaderTextured, "shadow_map", 1);
        SetUniform1Value(shaderTextured, "textureSampler", 0);
        SetUniformVec3(shaderSphere, "light_position", lightPosition);
        SetUniform1Value(shaderSphere, "shadow_map", 1);
        SetUniform1Value(shaderSphere, "textureSampler", 0);

        // Set light space matrix on both shaders
        SetUniformMat4(shaderShadow, "light_view_proj_matrix", lightSpaceMatrix);
        SetUniformMat4(shaderTextured, "light_view_proj_matrix", lightSpaceMatrix);
        SetUniformMat4(shaderSphere, "light_view_proj_matrix", lightSpaceMatrix);

        // Set light far and near planes on scene shader
        SetUniform1Value(shaderTextured, "light_near_plane", lightNearPlane);
        SetUniform1Value(shaderTextured, "light_far_plane", lightFarPlane);
        SetUniform1Value(shaderSphere, "light_near_plane", lightNearPlane);
        SetUniform1Value(shaderSphere, "light_far_plane", lightFarPlane);

        // Set light position on scene shader
        SetUniformVec3(shaderTextured, "light_position", lightPosition);
        SetUniformVec3(shaderSphere, "light_position", lightPosition);

        // Set light direction on scene shader
        SetUniformVec3(shaderTextured, "light_direction", lightDirection);
        SetUniformVec3(shaderSphere, "light_direction", lightDirection);

        // Set view position on scene shader
        SetUniformVec3(shaderTextured, "view_position", camera.Position);
        SetUniformVec3(shaderSphere, "view_position", camera.Position);



        GLuint worldMatrixLocation = glGetUniformLocation(shaderTextured, "worldMatrix");


        mat4 WorldRotationMatrix = translate(mat4(1.0f), vec3(-33.0f, 0.0f, 10.0f));
        WorldRotationMatrix = rotate(WorldRotationMatrix, radians(180.0f), vec3(0, 1, 0));
        // Set model matrix and send to both shaders
        mat4 modelMatrix = WorldRotationMatrix *
            glm::translate(mat4(1.0f), vec3(0.0f, 0.0f, -7.0f)) *
            glm::scale(mat4(1.0f), vec3(0.007f));

        SetUniformMat4(shaderTextured, "worldMatrix", modelMatrix);
        SetUniformMat4(shaderShadow, "worldMatrix", modelMatrix);

       
        mat4 tableBaseCubeMatrix = WorldRotationMatrix * translate(mat4(1.0f), vec3(0.0f, 2.0f, 0.0f)) * scale(mat4(1.0f), vec3(5.0f, 3.0f, 3.0f)) * scale(mat4(1.0f), vec3(0.5f, 0.5f, 0.5f));
        mat4 tableTopCubeMatrix = WorldRotationMatrix * translate(mat4(1.0f), vec3(0.0f, 3.5f, 0.0f)) * scale(mat4(1.0f), vec3(6.0f, 0.5f, 4.0f)) * scale(mat4(1.0f), vec3(0.5f, 0.5f, 0.5f));
        mat4 tableBottonCubeMatrix = WorldRotationMatrix * translate(mat4(1.0f), vec3(0.0f, 0.25f, 0.0f)) * scale(mat4(1.0f), vec3(6.0f, 0.5f, 4.0f)) * scale(mat4(1.0f), vec3(0.5f, 0.5f, 0.5f));
        mat4 bubbleMachine = WorldRotationMatrix * translate(mat4(1.0f), vec3(0.0f, 4.76f, 0.0f)) * rotate(mat4(1.0f), radians(spinningCubeAngle), vec3(0.0f, 1.0f, 0.0f)) * scale(mat4(1.0f), bubbleMachineScale) * scale(mat4(1.0f), vec3(0.5f, 0.5f, 0.5f));
        
        mat4 chairBase = WorldRotationMatrix * translate(mat4(1.0f), vec3(0.0f, 0.75f, -6.7f)) * scale(mat4(1.0f), vec3(0.4f, 0.7f, 0.4f)) * scale(mat4(1.0f), vec3(0.5f, 0.5f, 0.5f));
        mat4 chairMiddle = chairBase * translate(mat4(1.0f), vec3(0.0f, 3.0f, 0.0f)) * scale(mat4(1.0f), vec3(0.5f, 3.0f, 0.5f));
        mat4 chairTop1 = chairBase * translate(mat4(1.0f), vec3(0.0f, 6.0f, 1.0f)) * scale(mat4(1.0f), vec3(5.0f, 0.3f, 8.0f));
        mat4 chairTop2 = chairBase * translate(mat4(1.0f), vec3(0.0f, 13.5f, -13.3f)) * rotate(mat4(1.0f), radians(50.0f), vec3(1.0f, 0.0f, 0.0f)) * scale(mat4(1.0f), vec3(5.0f, 0.3f, 10.0f));
        mat4 chairbottom1 = chairBase * translate(mat4(1.0f), vec3(0.0f, -1.0f, -6.0f)) * rotate(mat4(1.0f), radians(-7.0f), vec3(1.0f, 0.0f, 0.0f)) * scale(mat4(1.0f), vec3(0.4f, 0.4f, 6.0f));
        mat4 chairbottom2 = chairBase * translate(mat4(1.0f), vec3(-6.0f, -1.0f, 0.0f)) * rotate(mat4(1.0f), radians(7.0f), vec3(0.0f, 0.0f, 1.0f)) * scale(mat4(1.0f), vec3(-6.0f, 0.4f, 0.4f));
        mat4 chairbottom3 = chairBase * translate(mat4(1.0f), vec3(6.0f, -1.0f, 0.0f)) * rotate(mat4(1.0f), radians(-7.0f), vec3(0.0f, 0.0f, 1.0f)) * scale(mat4(1.0f), vec3(6.0f, 0.4f, 0.4f));
        mat4 chairbottom4 = chairBase * translate(mat4(1.0f), vec3(0.0f, -1.0f, 6.0f)) * rotate(mat4(1.0f), radians(7.0f), vec3(1.0f, 0.0f, 0.0f)) * scale(mat4(1.0f), vec3(0.4f, 0.4f, -6.0f));

        // Render shadow in 2 passes: 1- Render depth map, 2- Render scene
        // 1- Render shadow map:
        // a- use program for shadows
        // b- resize window coordinates to fix depth map output size
        // c- bind depth map framebuffer to output the depth values
        {
            // Use proper shader
            glUseProgram(shaderShadow);
            // Use proper image output size
            glViewport(0, 0, DEPTH_MAP_TEXTURE_SIZE, DEPTH_MAP_TEXTURE_SIZE);
            // Bind depth map texture as output framebuffer
            glBindFramebuffer(GL_FRAMEBUFFER, depth_map_fbo);
            // Clear depth data on the framebuffer
            glClear(GL_DEPTH_BUFFER_BIT);
            if (shadowAlgorithm) {
                // shadow for obj model
                // Bind geometry
                glBindVertexArray(activeVAO);
                // Draw geometry
                glDrawElements(GL_TRIANGLES, activeVertices, GL_UNSIGNED_INT, 0);
                // Unbind geometry
                glBindVertexArray(0);
                GLuint worldMatrixLocation0 = glGetUniformLocation(shaderShadow, "worldMatrix");
                mat4 groundWorldMatrix0 = translate(mat4(1), vec3(0,6,0));
                // Bind geometry
                glBindVertexArray(texturedVbO);
                // Draw geometry
                glUniformMatrix4fv(worldMatrixLocation0, 1, GL_FALSE, &tableBaseCubeMatrix[0][0]);
                glDrawArrays(GL_TRIANGLES, 0, 36);
                glUniformMatrix4fv(worldMatrixLocation0, 1, GL_FALSE, &tableTopCubeMatrix[0][0]);
                glDrawArrays(GL_TRIANGLES, 0, 36);
                glUniformMatrix4fv(worldMatrixLocation0, 1, GL_FALSE, &tableBottonCubeMatrix[0][0]);
                glDrawArrays(GL_TRIANGLES, 0, 36);
                // Unbind geometry
                glBindVertexArray(0);
            }
        }

        glBindFramebuffer(GL_FRAMEBUFFER, 0);

        // 2- Render scene: a- bind the default framebuffer and b- just render like what we do normally
        {
            // Use proper shader
            glUseProgram(shaderTextured);
            // Use proper image output size
            // Side note: we get the size from the framebuffer instead of using WIDTH and HEIGHT because of a bug with highDPI displays
            int width, height;
            glfwGetFramebufferSize(window, &width, &height);
            glViewport(0, 0, width, height);
            // Bind screen as output framebuffer
            glBindFramebuffer(GL_FRAMEBUFFER, 0);
            // Clear color and depth data on framebuffer
            glClearColor(0.8f, 0.8f, 0.8f, 1.0f);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
            // Bind depth map texture: not needed, by default it is active
            //glActiveTexture(GL_TEXTURE0);
            // Bind geometry
            glBindVertexArray(activeVAO);
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, xTextureID);
            // Draw geometry
            glDrawElements(GL_TRIANGLES, activeVertices, GL_UNSIGNED_INT, 0);
            // Unbind geometry
            glBindVertexArray(0);
            //// Use proper shader
            glUseProgram(shaderTextured);
            // draw a large world cube
            GLuint worldMatrixLocation0 = glGetUniformLocation(shaderTextured, "worldMatrix");
            mat4 groundWorldMatrix0 = translate(mat4(1), vec3(0, 6, 0));
            glBindVertexArray(texturedVbO);
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, woodTextureID);
            glActiveTexture(GL_TEXTURE1);
            glBindTexture(GL_TEXTURE_2D, depth_map_texture);
            glUniformMatrix4fv(worldMatrixLocation0, 1, GL_FALSE, &tableBaseCubeMatrix[0][0]);
            glDrawArrays(GL_TRIANGLES, 0, 36);
            glUniformMatrix4fv(worldMatrixLocation0, 1, GL_FALSE, &tableTopCubeMatrix[0][0]);
            glDrawArrays(GL_TRIANGLES, 0, 36);
            glUniformMatrix4fv(worldMatrixLocation0, 1, GL_FALSE, &tableBottonCubeMatrix[0][0]);
            glDrawArrays(GL_TRIANGLES, 0, 36);
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, carrotTextureID); glUniformMatrix4fv(worldMatrixLocation0, 1, GL_FALSE, &bubbleMachine[0][0]);
            glDrawArrays(GL_TRIANGLES, 0, 36);
            // draw chair
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, blackTextureID);
            glUniformMatrix4fv(worldMatrixLocation0, 1, GL_FALSE, &chairBase[0][0]);
            glDrawArrays(GL_TRIANGLES, 0, 36);
            glBindVertexArray(0);
            glBindVertexArray(texturedVbO);
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, silverTextureID);
            glUniformMatrix4fv(worldMatrixLocation0, 1, GL_FALSE, &chairMiddle[0][0]);
            glDrawArrays(GL_TRIANGLES, 0, 36);
            glBindVertexArray(0);
            glBindVertexArray(texturedVbO);
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, blackTextureID);
            glUniformMatrix4fv(worldMatrixLocation0, 1, GL_FALSE, &chairTop1[0][0]);
            glDrawArrays(GL_TRIANGLES, 0, 36);
            glUniformMatrix4fv(worldMatrixLocation0, 1, GL_FALSE, &chairTop2[0][0]);
            glDrawArrays(GL_TRIANGLES, 0, 36);
            glUniformMatrix4fv(worldMatrixLocation0, 1, GL_FALSE, &chairbottom1[0][0]);
            glDrawArrays(GL_TRIANGLES, 0, 36);
            glUniformMatrix4fv(worldMatrixLocation0, 1, GL_FALSE, &chairbottom2[0][0]);
            glDrawArrays(GL_TRIANGLES, 0, 36);
            glUniformMatrix4fv(worldMatrixLocation0, 1, GL_FALSE, &chairbottom3[0][0]);
            glDrawArrays(GL_TRIANGLES, 0, 36);
            glUniformMatrix4fv(worldMatrixLocation0, 1, GL_FALSE, &chairbottom4[0][0]);
            glDrawArrays(GL_TRIANGLES, 0, 36);
            // Unbind geometry
            glBindVertexArray(0);
            // Unbind geometry
            glUseProgram(shaderSphere);
            glBindVertexArray(ballVAO);
            //glActiveTexture(GL_TEXTURE0);
            //glBindTexture(GL_TEXTURE_2D, zTextureID);
            if (lastKeyQState == GLFW_RELEASE && glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS) {

                    spinningCubeAngle += 30.0f;
                    bubbleMachineScale = vec3(1.5f, 1.5f, 1.5f);
                    const float projectileSpeed = 15.0f;
                    bubbleList.push_back(Bubble(vec3(0.0f, 4.76f, 0.0f), projectileSpeed * camera.Forward, shaderTextured));


            }

            if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_RELEASE) {

                    bubbleMachineScale = vec3(2.0f, 2.0f, 2.0f);
                

            }
            for (list<Bubble>::iterator it = bubbleList.begin(); it != bubbleList.end(); it++) {
                it->Update(dt);
                it->Draw();
            }
            lastKeyQState = glfwGetKey(window, GLFW_KEY_Q);
            glBindVertexArray(0);
        }

        SetUniformMat4(shaderTextured, "viewMatrix", viewMatrix);
        SetUniformMat4(shaderSphere, "viewMatrix", viewMatrix);// light parameters
    }

            


    GLuint setupModelVBO(string path, int& vertexCount)
    {
        std::vector<glm::vec3> vertices;
        std::vector<glm::vec3> normals;
        std::vector<glm::vec2> UVs;

        //read the vertex data from the model's OBJ file
        loadOBJ(path.c_str(), vertices, normals, UVs);

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

        glBindVertexArray(0);
        // Unbind VAO (it's always a good thing to unbind any buffer/array to prevent strange bugs, as we are using multiple VAOs)
        vertexCount = vertices.size();
        return VAO;
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



};

