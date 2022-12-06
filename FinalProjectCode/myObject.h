#pragma once


#include <iostream>
#include <vector>
#include <list>
#include <algorithm>
#include <string>
#include <sstream>
//#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"


#include "Shader.h"
#include "tiny_obj_loader.h"



#define GLEW_STATIC 1   // This allows linking with Static Library on Windows, without DLL
#include <GL/glew.h>    // Include GLEW - OpenGL Extension Wrangler

#include <GLFW/glfw3.h> // GLFW provides a cross-platform interface for creating a graphical context,
						// initializing OpenGL and binding inputs

#include <glm/glm.hpp>  // GLM is an optimized math library with syntax to similar to OpenGL Shading Language
#include <glm/gtc/matrix_transform.hpp> // include this to create transformation matrices
#include <glm/common.hpp>

using namespace glm;
using namespace std;

typedef struct {
	GLuint *animateTextures;
	GLuint frames;
} GifTexture;
typedef struct {
	GLuint vb_id;  // vertex buffer id
	int numTriangles;
	size_t material_id;
	
} DrawObject;

enum ColorType { TEXTURE, SINGLE, GRADIENT };
class MyModels {
public:
	float bmin[3], bmax[3];
	std::vector<tinyobj::material_t> materials;
	std::map<std::string, GLuint> textures;
	std::vector<DrawObject> drawObjects;
	std::vector<GifTexture> gifTextures;
	
	

	ColorType colorType;
	glm::vec3 solidColor;
	glm::mat4 scaleMat;
	glm::mat4 positionMat;
	glm::mat4 translateMat;
	MyModels(){
		solidColor = glm::vec3(1);
		colorType = TEXTURE;
		positionMat = glm::mat4(1);
		scaleMat = glm::mat4(1);
		translateMat = glm::mat4(1);
	}
	MyModels( glm::mat4 position) {
		
		this->positionMat = glm::mat4(1);
		scaleMat = glm::mat4(1);
		translateMat = glm::mat4(1);
	}
	glm::mat4 getModelView() {

		float maxExtent = 0.5f * (bmax[0] - bmin[0]);
		if (maxExtent < 0.5f * (bmax[1] - bmin[1])) {
			maxExtent = 0.5f * (bmax[1] - bmin[1]);
		}
		if (maxExtent < 0.5f * (bmax[2] - bmin[2])) {
			maxExtent = 0.5f * (bmax[2] - bmin[2]);
		}
		// * translateMat * glm::scale(glm::mat4(1), glm::vec3(0.3f, 0.3f, 0.3f))
		return  positionMat * translateMat * glm::scale(glm::mat4(1), glm::vec3(1.0f / maxExtent, 1.0f / maxExtent, 1.0f / maxExtent)) * scaleMat;
	}
	void Draw(Shader& shader,int nframes) {
		GLsizei stride = (3 + 3 + 3 + 2) * sizeof(float);
		for (size_t i = 0; i < drawObjects.size(); i++) {
			DrawObject o = drawObjects[i];
			if (o.vb_id < 1) {
				continue;
			}

			glBindVertexArray(o.vb_id);


			glBindTexture(GL_TEXTURE_2D, 0);
			/*if ((o.material_id < materials.size())) {
				std::string diffuse_texname = materials[o.material_id].diffuse_texname;
				if (textures.find(diffuse_texname) != textures.end()) {
					glBindTexture(GL_TEXTURE_2D, textures[diffuse_texname]);
				}
			}*/
			if (gifTextures.size() > 0) {
				GifTexture gifTexture = gifTextures[0];
				GLuint texture = gifTexture.animateTextures[nframes%gifTexture.frames];
				glBindTexture(GL_TEXTURE_2D, texture);
			}
			else {
				if ((o.material_id < materials.size())) {
					std::string diffuse_texname = materials[o.material_id].diffuse_texname;
					if (textures.find(diffuse_texname) != textures.end()) {
						glBindTexture(GL_TEXTURE_2D, textures[diffuse_texname]);
					}
				}
			}

			auto model = getModelView();
			shader.setVec3("solidColor", solidColor);
			shader.setMat4("model", model);
			shader.setInt("colorType", colorType);
			glDrawArrays(GL_TRIANGLES, 0, 3 * o.numTriangles);
			
			glBindTexture(GL_TEXTURE_2D, 0);
		}
	}
};


class MyObject {
public:
	int shaderProgram;
	float lastFrameTime;
	int nframe;
	GLFWwindow* window = NULL;
	vector<MyModels> myDrawModels;
	int currentModels;
	GLuint loadTexture(const char* filename);
	unsigned int planeVAO;
	GLuint floorTexture;
public:
	void updateObjColor(ColorType ct);
	void Update(float dt);
	MyObject(GLFWwindow* window);
	void Draw(Shader& shader, float dt);
	void loadObjs();
	void createFloorVao();
	
	
};



















