#pragma once

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <stb_image.h>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include "Shader.h"
#include "Mesh.h"

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <map>
#include <vector>

uint32_t TextureFromFile(const char* path, const std::string& directory, bool gamma = false);



// Клас Model представляє модель, яка містить вектори текстур і мешів, шлях до директорії моделі та прапорець для включення корекції гами.
class Model {
public:
	std::vector<Texture> textures_loaded; // Вектор для зберігання завантажених текстур
	std::vector<Mesh> meshes; // Вектор для зберігання мешів
	std::string directory; // Шлях до директорії моделі
	bool gammaCorrection; // Прапорець для включення корекції гами

	
    //Конструктор для завантаження моделі з файлу
	Model(const std::string& path, bool gamma = false);

	Model() = default;

	bool operator==(const Model& other) const;

	
    // Метод для відображення моделі за допомогою шейдера.
	void Draw(Shader& shader);

private:
	
    // Метод для завантаження моделі з файлу.
	void loadModel(const std::string& path);

	
    // Метод для обробки вузла моделі.
	void processNode(aiNode* node, const aiScene* scene);

	
    // Метод для обробки мешу моделі.
	Mesh processMesh(aiMesh* mesh, const aiScene* scene);

	
    // Метод для завантаження текстур матеріалу.
	std::vector<Texture> loadMaterialTextures(aiMaterial* mat, aiTextureType type, std::string typeName);
};