#pragma once
#include "Mesh.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

class Model
{
public:
    /*  函数   */
    Model();
    Model(std::string path);
    void Draw(Shader shader);
    void updatPosition();
    float deltX = 0.0f;
private:
    /*  模型数据  */
    std::vector<Texture> textures_loaded;
    std::vector<Mesh> meshes;
    std::string directory;
    /*  函数   */
    void loadModel(std::string path);
    void processNode(aiNode* node, const aiScene* scene);
    Mesh processMesh(aiMesh* mesh, const aiScene* scene);
    std::vector<Texture> loadMaterialTextures(aiMaterial* mat, aiTextureType type, std::string typeName);
};

