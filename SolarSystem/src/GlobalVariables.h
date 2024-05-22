#pragma once

#include "Camera.h"

static const GLuint SCR_WIDTH = 1920;
static const GLuint SCR_HEIGHT = 1080;

static const GLuint SHADOW_WIDTH = 1024, SHADOW_HEIGHT = 1024;

static const glm::vec3 lightPos(0.0f, 0.0f, 0.0f);

static const std::vector<float> planetSpeedAroundSun{ 1.f, 1.3f, 1.6f, 2.0f, 2.5f, 3.0f, 3.5f, 4.0f };