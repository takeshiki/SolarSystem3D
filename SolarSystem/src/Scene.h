#pragma once

#include <vector>
#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "Model.h"
#include "GlobalVariables.h"

enum SpaceObjectType
{
	SUN = 0,
	MOON = 2,
	SATURN_CIRCLES = 5,
	SPACE_MAP = 1
};

enum PlanetType
{
	MERCURY = 0,
	VENUS = 1,
	EARTH = 2,
	MARS = 3,
	JUPITER = 4,
	SATURN = 5,
	URANUS = 6,
	NEPTUNE = 7
};

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
	
	Scene(std::vector<Model>& planets, std::map<int, Model>& spaceObjects, Shader& shaderDefault, Shader& shaderSunlight, Shader& shaderShadow);

	void renderScene(glm::mat4 cameraViewMatrix, glm::vec3& cameraPosition);

	void renderSceneObjects(glm::mat4& cameraViewMatrix);
};

