#pragma once

#include <vector>
#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "Model.h"
#include "GlobalVariables.h"

class Scene
{
private:
	std::vector<Model> m_planets;
	std::map<int, Model> m_spaceObjects;
	GLuint depthMapFBO, depthMap;
	Shader m_shaderShadow;
	Shader m_shaderSunlight;
	Shader m_shaderDefault;

	void bindModelVAO(const Model& model);
	void bindDepthMapFBO();

	glm::mat4 generateLightSpaceMatrix();
	void renderShadow(glm::mat4 lightSpaceMatrix);
public:
	
	Scene(std::vector<Model> planets, std::map<int, Model> spaceObjects);

	void renderScene();

	void renderSceneObjects();
};

