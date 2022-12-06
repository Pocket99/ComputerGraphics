#include "ProjectMove.h"

GLuint ProjectMove::loadTexture(const char* filename)
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

ProjectMove::ProjectMove()
{
    // Load Textures
    hatTextureID = loadTexture("assets/textures/hat.jpg");
    skyTextureID = loadTexture("assets/textures/sky.jpg");
    fireTextureID = loadTexture("assets/textures/fire.jpg");
    redTextureID = loadTexture("assets/textures/red.jpg");

    //Setup models
    carPath = "assets/models/car.obj";
    balloonPath = "assets/models/balloon.obj";

    carVAOVertices;
    carVAO = setupModelVBO(carPath, carVAOVertices);

    balloonVAOVertices;
    balloonVAO = setupModelVBO(balloonPath, balloonVAOVertices);
}

void ProjectMove::Draw(Shader* shader, float dt)
{
    //bind car vao
    glBindVertexArray(0);
    glBindVertexArray(carVAO);

    //draw car
    glBindTexture(GL_TEXTURE_2D, 0);
    glBindTexture(GL_TEXTURE_2D, hatTextureID);
    mat4 modelWorldMatrix =
        translate(mat4(1.0f), vec3(moveX, moveY, moveZ)) *
        glm::translate(mat4(1.0f), vec3(-10.0f, 3.0f, 12.0f)) *
        glm::rotate(mat4(1.0f), radians(spinningAngle), vec3(0.0f, 1.0f, 0.0f)) *
        //glm::rotate(mat4(1.0f), radians(-90.0f), vec3(1.0f, 0.0f, 0.0f)) *
        glm::rotate(mat4(1.0f), radians(spinningAngleY), vec3(1.0f, 0.0f, 0.0f)) *
        glm::scale(mat4(1.0f), vec3(1.5f));

    shader->setMat4("modelMat", modelWorldMatrix);
    glDrawArrays(GL_TRIANGLES, 0, carVAOVertices);

    //draw balloon
    //bind balloon vao
    spinningAngleB += 20.0f * dt;
    glBindVertexArray(0);
    glBindVertexArray(balloonVAO);

    glBindTexture(GL_TEXTURE_2D, 0);
    glBindTexture(GL_TEXTURE_2D, skyTextureID);
    modelWorldMatrix =
        //translate(mat4(1.0f), vec3(moveX, moveY, moveZ)) *
        glm::translate(mat4(1.0f), vec3(6.0f-10, 2.0f+3, -5.0f+12)) *
        glm::rotate(mat4(1.0f), radians(spinningAngleB), vec3(0.0f, 1.0f, 0.0f)) *
        glm::scale(mat4(1.0f), vec3(0.1f));
    shader->setMat4("modelMat", modelWorldMatrix);
    glDrawArrays(GL_TRIANGLES, 0, balloonVAOVertices);

    glBindVertexArray(0);
    glBindVertexArray(balloonVAO);

    //draw second balloon
    spinningAngleS += 10.0f * dt;
    glBindTexture(GL_TEXTURE_2D, 0);
    glBindTexture(GL_TEXTURE_2D, fireTextureID);
    modelWorldMatrix =
        //translate(mat4(1.0f), vec3(moveX, moveY, moveZ)) *
        glm::translate(mat4(1.0f), vec3(8.0f-10, 1.5f+3, -6.0f+12)) *
        glm::rotate(mat4(1.0f), radians(-spinningAngleS), vec3(0.0f, 1.0f, 0.0f)) *
        glm::scale(mat4(1.0f), vec3(0.05f));
    shader->setMat4("modelMat", modelWorldMatrix);
    glDrawArrays(GL_TRIANGLES, 0, balloonVAOVertices);

    //draw third balloon

    glBindTexture(GL_TEXTURE_2D, 0);
    glBindTexture(GL_TEXTURE_2D, redTextureID);
    modelWorldMatrix =
        //translate(mat4(1.0f), vec3(moveX, moveY, moveZ)) *
        glm::translate(mat4(1.0f), vec3(3.0f-10, 1.7f+3, -6.0f+12)) *
        glm::rotate(mat4(1.0f), radians(-spinningAngleS), vec3(0.0f, 1.0f, 0.0f)) *
        glm::scale(mat4(1.0f), vec3(0.05f));
    shader->setMat4("modelMat", modelWorldMatrix);
    glDrawArrays(GL_TRIANGLES, 0, balloonVAOVertices);
}


