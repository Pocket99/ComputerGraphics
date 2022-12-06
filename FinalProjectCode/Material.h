#pragma once
#include"Shader.h"

class Material
{
public:
	Shader* shader;
	unsigned int diffuse;
	unsigned int specular;
	unsigned int emission;
	glm::vec3 ambient;
	float shininess;
	Material(Shader* shader, unsigned int diffuse, unsigned int specular, glm::vec3 ambient, float shininess);
	~Material();

};

