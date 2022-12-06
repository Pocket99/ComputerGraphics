#ifndef SHADER_H
#define SHADER_H
#include<string>
#include <fstream>
#include <sstream>
#include <iostream>
#define GLEW_STATIC
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
class Shader
{
public:
	Shader(const char* vertexPath, const char* fragmentPath);
	void use();
	void setUniform3f(const char* paraNameString, glm::vec3 param);
	void setUniform1f(const char* paraNameString, float param);
	void setUniform1i(const char* paraNameString, int slot);
	void setBool(const std::string& name, bool value) const;
	void setInt(const std::string& name, int value) const;
	void setFloat(const std::string& name, float value) const;
	void setVec3(const std::string& name, glm::vec3 value) const;
	void setMat4(const std::string& name, glm::mat4 value) const;
	//~Shader();
public:
	unsigned int ID;
	enum Slot
	{
		DIFFUSE,
		SPECULAR,
		EMISSION
	};
private:
	void checkCompileErrors(unsigned int ID, std::string type);
};
#endif
