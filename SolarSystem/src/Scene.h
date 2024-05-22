#pragma once

#include<vector>

#include "Model.h"

const std::vector<float> planetSpeedAroundSun { 1.f, 1.3f, 1.6f, 2.0f, 2.5f, 3.0f, 3.5f, 4.0f };

const GLuint SHADOW_WIDTH = 1024, SHADOW_HEIGHT = 1024;

class Scene
{
private:
	std::vector<Model> m_planets;
	std::map<int, Model> m_spaceObjects;
	GLuint depthMapFBO, depthMap;
public:
	void bindModelVAO(const Model& model);
	void bindDepthMapFBO();
	
	Scene(std::vector<Model> planets, std::map<int, Model> spaceObjects);
};

