#pragma once


#include <iostream>
#include <vector>
#include <list>
#include <algorithm>
#include <string>
#include <sstream>
//#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

//#include "OBJloaderV2.h"  //For loading .obj files using a polygon list format
#include "Shader.h"

using namespace glm;
using namespace std;

#include <glm/glm.hpp>
#include <cstring>
#include <stdio.h>
#include <stdlib.h>






class ProjectMove {
public:
    GLuint loadTexture(const char* filename);

    mat4 bodyWorldMatrix;
    GLuint worldMatrixLocation;

    float lastFrameTime = glfwGetTime();

    float spinningAngle = 0.0f;
    float spinningAngleB = 0.0f;
    float spinningAngleS = 0.0f;
    float spinningAngleY = 0.0f;
    float moveX = 0.0f;
    float moveY = 0.0f;
    float moveZ = 0.0f;
    bool stopPoint = false;

    int shaderProgram;

    GLFWwindow* window = NULL;


    //draw scene use variables
    int texturedCubeVAO;
    int texturedShaderProgram;
    int activeVAOVertices;
    GLuint activeVAO;

    GLuint hatTextureID;
    GLuint skyTextureID;
    GLuint redTextureID;
    GLuint fireTextureID;

    string carPath;
    string balloonPath ;

    int carVAOVertices;
    GLuint carVAO ;

    int balloonVAOVertices;
    GLuint balloonVAO;


    ProjectMove();
    void Draw(Shader* shader, float dt);
    
    GLuint setupModelVBO(string path, int& vertexCount) {
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

        glBindVertexArray(0); // Unbind VAO (it's always a good thing to unbind any buffer/array to prevent strange bugs, as we are using multiple VAOs)
        vertexCount = vertices.size();
        return VAO;
    }

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
};










        


        





