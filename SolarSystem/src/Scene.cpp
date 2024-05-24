#include "Scene.h"

void Scene::bindModelVAO(const Model& model)
{// прив'язка VAO для моделі
    for (GLuint i = 0; i < model.meshes.size(); i++)
    {
        GLuint VAO = model.meshes[i].VAO;
        glBindVertexArray(VAO);
        // set attribute pointers for matrix (4 times vec4)
        glEnableVertexAttribArray(3);
        glVertexAttribPointer(3, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*)0);
        glEnableVertexAttribArray(4);
        glVertexAttribPointer(4, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*)(sizeof(glm::vec4)));
        glEnableVertexAttribArray(5);
        glVertexAttribPointer(5, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*)(2 * sizeof(glm::vec4)));
        glEnableVertexAttribArray(6);
        glVertexAttribPointer(6, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*)(3 * sizeof(glm::vec4)));

        glVertexAttribDivisor(3, 1);
        glVertexAttribDivisor(4, 1);
        glVertexAttribDivisor(5, 1);
        glVertexAttribDivisor(6, 1);

        glBindVertexArray(0);
    }
}

void Scene::bindDepthMapFBO()
{// створити FBO для тіней
    glGenFramebuffers(1, &depthMapFBO);
    // create depth texture
    glGenTextures(1, &depthMap);
    glBindTexture(GL_TEXTURE_2D, depthMap);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, SHADOW_WIDTH, SHADOW_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
    float borderColor[] = { 1.0, 1.0, 1.0, 1.0 };
    glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);
    // attach depth texture as FBO's depth buffer
    glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthMap, 0);
    glDrawBuffer(GL_NONE);
    glReadBuffer(GL_NONE);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

}

glm::mat4 Scene::generateLightSpaceMatrix()
{// згенерувати матрицю для тіней
    glm::mat4 lightProjection, lightView;
    glm::mat4 lightSpaceMatrix;
    float near_plane = 1.0f, far_plane = 7.5f;
    //lightProjection = glm::perspective(glm::radians(45.0f), (GLfloat)SHADOW_WIDTH / (GLfloat)SHADOW_HEIGHT, near_plane, far_plane); // note that if you use a perspective projection matrix you'll have to change the light position as the current light position isn't enough to reflect the whole scene
    lightProjection = glm::ortho(-10.0f, 10.0f, -10.0f, 10.0f, near_plane, far_plane);
    lightView = glm::lookAt(lightPos, glm::vec3(0.0f), glm::vec3(0.0, 1.0, 0.0));
    lightSpaceMatrix = lightProjection * lightView;
    return lightSpaceMatrix;
}

void Scene::renderShadow(glm::mat4 lightSpaceMatrix)
{// рендер тіней з точки зору світла
    m_shaderShadow.use();
    m_shaderShadow.set("lightSpaceMatrix", lightSpaceMatrix);

    glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);
    glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
    glClear(GL_DEPTH_BUFFER_BIT);
    glActiveTexture(GL_TEXTURE0);
}

Scene::Scene(std::vector<Model>& planets, std::map<int, Model>& spaceObjects, Shader& shaderDefault, Shader& shaderSunlight, Shader& shaderShadow)
    : m_planets(planets), m_spaceObjects(spaceObjects),
    m_shaderDefault(shaderDefault), m_shaderSunlight(shaderSunlight), m_shaderShadow(shaderShadow) 
{// конструктор сцени

	for (auto& planet : planets)
	{
		bindModelVAO(planet);
	}
	for (auto& spaceObject : spaceObjects)
	{
		bindModelVAO(spaceObject.second);
	}
    
    bindDepthMapFBO();

    m_shaderSunlight.use();
    m_shaderSunlight.set("diffuseTexture", 0);
    m_shaderSunlight.set("shadowMap", 1);
}

void Scene::renderScene(glm::mat4 cameraViewMatrix, glm::vec3& cameraPosition)
{// рендер сцени
    glm::mat4 lightSpaceMatrix = generateLightSpaceMatrix();
    renderShadow(lightSpaceMatrix);

    m_shaderSunlight.use();
    glm::mat4 projection = glm::perspective(glm::radians(45.0f), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100000.0f);
    glm::mat4 view = cameraViewMatrix;
    glm::mat4 model = glm::mat4(1.0f);
    m_shaderSunlight.set("projection", projection);
    m_shaderSunlight.set("view", view);

    renderSceneObjects(cameraViewMatrix); // render before shadow

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glViewport(0, 0, SCR_WIDTH, SCR_HEIGHT);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    m_shaderSunlight.set("viewPos", cameraPosition);
    m_shaderSunlight.set("lightPos", lightPos);
    m_shaderSunlight.set("lightSpaceMatrix", lightSpaceMatrix);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, depthMap);

    renderSceneObjects(cameraViewMatrix); // render after shadow

    m_shaderDefault.use();
    m_shaderDefault.set("projection", projection);
    m_shaderDefault.set("view", view);

    model = glm::mat4(1.0f);// draw sun
    model = glm::translate(model, glm::vec3(0.0f, 0.0f, 0.f));
    model = glm::scale(model, glm::vec3(6.0f, 6.0f, 6.0f));
    m_shaderDefault.set("model", model);
    m_spaceObjects[SUN].Draw(m_shaderDefault);

    glCullFace(GL_FRONT);
    model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(cameraPosition));
    model = glm::scale(model, glm::vec3(100.0f, 100.0f, 100.0f));
    m_shaderDefault.set("model", model);
    m_spaceObjects[SPACE_MAP].Draw(m_shaderDefault);
}

void Scene::renderSceneObjects(glm::mat4& cameraViewMatrix)
{// рендер об'єктів сцени
    m_shaderSunlight.use();
    glm::mat4 projection = glm::perspective(glm::radians(45.0f), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100000.0f);
    glm::mat4 view = cameraViewMatrix;
    glm::mat4 model = glm::mat4(1.0f);
    m_shaderSunlight.set("projection", projection);
    m_shaderSunlight.set("view", view);

    float distance = 100.0f;
    int counterForPlanetNumber = 0;
    for (auto& planet : m_planets)
    {
        model = glm::mat4(1.0f);
        distance += 100.0f;
        // рухати планети по колу
        model = glm::translate(model, glm::vec3((cos(glfwGetTime() / planetSpeedAroundSun[counterForPlanetNumber]) * distance), 0.0f, sin(glfwGetTime() / planetSpeedAroundSun[counterForPlanetNumber]) * distance));
        if (counterForPlanetNumber == EARTH || counterForPlanetNumber == MARS || counterForPlanetNumber == SATURN) {
            model = glm::rotate(model, glm::radians(static_cast<float>(glfwGetTime()) * 20.0f), glm::vec3(0.2f, 1.0f, 0.0f)); // Rotate around Y-axis
        }//якщо земля, марс або сатурн то переміщаємо вісь обертання
        else
        {
            model = glm::rotate(model, glm::radians(static_cast<float>(glfwGetTime()) * 20.0f), glm::vec3(0.0f, 1.0f, 0.0f)); // Rotate around Y-axis
        }
        model = glm::scale(model, glm::vec3(4.0f, 4.0f, 4.0f));
        m_shaderSunlight.set("model", model);
        planet.Draw(m_shaderSunlight);
        if (counterForPlanetNumber == SATURN) {
            m_spaceObjects[SATURN_CIRCLES].Draw(m_shaderSunlight);
        }
        if (counterForPlanetNumber == EARTH) {
            model = glm::mat4(1.0f);
            model = glm::translate(model, glm::vec3((cos(glfwGetTime() / planetSpeedAroundSun[2]) * distance) + 30.f, 30.0f, sin(glfwGetTime() / planetSpeedAroundSun[2]) * distance));
            model = glm::scale(model, glm::vec3(8.0f, 8.0f, 8.0f));
            m_shaderSunlight.set("model", model);
            m_spaceObjects[MOON].Draw(m_shaderSunlight);
        }
        counterForPlanetNumber++;
    }
}
