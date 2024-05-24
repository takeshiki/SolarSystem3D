#pragma once
#include <glad/glad.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "Shader.h"

#include <string>
#include <vector>

#define MAX_BONE_INFLUENCE 4

struct Vertex {
    glm::vec3 Position;
    glm::vec3 Normal;
    glm::vec2 TexCoords;
    glm::vec3 Tangent;
    glm::vec3 Bitangent;
    // кістки, які впливають на вершину
    int m_BoneIDs[MAX_BONE_INFLUENCE];
    // ваги кісток, які впливають на вершину
    float m_Weights[MAX_BONE_INFLUENCE];
};

struct Texture {
    unsigned int id;
    std::string type;
    std::string path;
};

class Mesh {
public:
    std::vector<Vertex>     vertices;
    std::vector<uint32_t>   indices;
    std::vector<Texture>    textures;
    uint32_t VAO;

    Mesh(std::vector<Vertex> vertices, std::vector<uint32_t> indices, std::vector<Texture> textures);

    void Draw(Shader& shader);
private:
    uint32_t VBO, EBO;

    void setupMesh();
};