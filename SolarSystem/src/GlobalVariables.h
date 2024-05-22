#pragma once

#include "Camera.h"

static const GLuint SCR_WIDTH = 1920;
static const GLuint SCR_HEIGHT = 1080;

static Camera camera(glm::vec3(-200.0f, 200.0f, 100.0f));
static float lastX = SCR_WIDTH / 2.0f;
static float lastY = SCR_HEIGHT / 2.0f;
static bool firstMouse = true;

// timing
static float deltaTime = 0.0f;	// time between current frame and last frame
static float lastFrame = 0.0f;


static const GLuint SHADOW_WIDTH = 1024, SHADOW_HEIGHT = 1024;

static const glm::vec3 lightPos(0.0f, 0.0f, 0.0f);

static const std::vector<float> planetSpeedAroundSun{ 1.f, 1.3f, 1.6f, 2.0f, 2.5f, 3.0f, 3.5f, 4.0f };