#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/gtc/type_ptr.hpp>

#include "Shader.h"
#include "Camera.h"
#include "Model.h"

#include <iostream>
#include <map>

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow* window);
void mouse_callback(GLFWwindow* window, double xposIn, double yposIn);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void bindModelVAO(Model model);
void bindDepthMapFBO(unsigned int& depthMapFBO, unsigned int& depthMap);
void createFPS();
glm::mat4 generateLightSpaceMatrix(glm::vec3 lightPos);

const unsigned int SCR_WIDTH = 1920; 
const unsigned int SCR_HEIGHT = 1080;

Camera camera(glm::vec3(-200.0f, 200.0f, 100.0f));
float lastX = SCR_WIDTH / 2.0f;
float lastY = SCR_HEIGHT / 2.0f;
bool firstMouse = true;

// timing
float deltaTime = 0.0f;	// time between current frame and last frame
float lastFrame = 0.0f;


const unsigned int SHADOW_WIDTH = 1024, SHADOW_HEIGHT = 1024;


int main(int, char**) {
    
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 4);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_SAMPLES, 4);

    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "thing", NULL, NULL);
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetScrollCallback(window, scroll_callback);

    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    glEnable(GL_DEPTH_TEST); 
    glEnable(GL_CULL_FACE);
    
    Shader shaderSunLight("src/glsl/sunlight_vertex.glsl", "src/glsl/sunlight_fragment.glsl");
    Shader shaderShadow("src/glsl/shadow_vertex.glsl", "src/glsl/shadow_fragment.glsl");
    Shader shaderPlanet("src/glsl/planet_vertex.glsl", "src/glsl/planet_fragment.glsl");
    
    
    Model sun("objects/sun/sun.obj");
    Model mercury("objects/mercury/mercury.obj");
    Model venus("objects/venus/venus.obj");
    Model earth("objects/earth/earth.obj");
    Model mars("objects/mars/mars.obj");
    Model jupiter("objects/jupiter/jupiter.obj");
    Model saturn("objects/saturn/saturn.obj");
    Model uranus("objects/uranus/uranus.obj");
    Model neptune("objects/neptune/neptune.obj");
    
    Model spaceMap("objects/space/space.obj");
    
    std::vector<Model> planets{ mercury, venus, earth, mars, jupiter, saturn, uranus, neptune };
    std::vector<float> planetSpeedAroundSun { 1.f, 1.3f, 1.6f, 2.0f, 2.5f, 3.0f, 3.5f, 4.0f };
    
    
    bindModelVAO(sun);

    for (const auto& planet : planets)
    {
        bindModelVAO(planet);
    }
    
    
    unsigned int depthMapFBO, depthMap;
    bindDepthMapFBO(depthMapFBO, depthMap);
    
    shaderSunLight.use();
    shaderSunLight.set("diffuseTexture", 0);
    shaderSunLight.set("shadowMap", 1);

    glm::vec3 lightPos(0.0f, 0.0f, 0.0f);

    while (!glfwWindowShouldClose(window))
    {

        createFPS();

        processInput(window);


        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glCullFace(GL_BACK);
        

        auto lightSpaceMatrix = generateLightSpaceMatrix(lightPos);

        // render scene from light's point of view
        shaderShadow.use();
        shaderShadow.set("lightSpaceMatrix", lightSpaceMatrix);

        glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);
        glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
        glClear(GL_DEPTH_BUFFER_BIT);
        glActiveTexture(GL_TEXTURE0);
       
        
        shaderSunLight.use();
        glm::mat4 projection = glm::perspective(glm::radians(45.0f), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 60000.0f);
        glm::mat4 view = camera.GetViewMatrix();
        glm::mat4 model = glm::mat4(1.0f);
        shaderSunLight.set("projection", projection);
        shaderSunLight.set("view", view);

        
        
        
        float distance = 100.0f;
        int counterForPlanetNumber = 0;
        for (auto& planet : planets)
        {
            model = glm::mat4(1.0f);
            distance += 100.0f;
            // Move in a circular path
            model = glm::translate(model, glm::vec3((cos(glfwGetTime()/ planetSpeedAroundSun[counterForPlanetNumber]) * distance), 0.0f, sin(glfwGetTime() / planetSpeedAroundSun[counterForPlanetNumber]) * distance));
            //model = glm::translate(model, glm::vec3(0.0f, 0.0f, distance));
            if (planet == earth && planet == mars && planet == saturn) {
                model = glm::rotate(model, glm::radians(static_cast<float>(glfwGetTime()) * 20.0f), glm::vec3(0.2f, 1.0f, 0.0f)); // Rotate around Y-axis
            }
            else
            {
                model = glm::rotate(model, glm::radians(static_cast<float>(glfwGetTime()) * 20.0f), glm::vec3(0.0f, 1.0f, 0.0f)); // Rotate around Y-axis
            }
            model = glm::scale(model, glm::vec3(4.0f, 4.0f, 4.0f)); 
            shaderSunLight.set("model", model);
            planet.Draw(shaderSunLight);
            counterForPlanetNumber++;
		}
        
        
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        glViewport(0, 0, SCR_WIDTH, SCR_HEIGHT);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        shaderSunLight.use();
        projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 60000.0f);
        view = camera.GetViewMatrix();
        shaderSunLight.set("projection", projection);
        shaderSunLight.set("view", view);

        // set light uniforms
        shaderSunLight.set("viewPos", camera.Position);
        shaderSunLight.set("lightPos", lightPos);
        shaderSunLight.set("lightSpaceMatrix", lightSpaceMatrix);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, depthMap);

        distance = 100.0f;
        counterForPlanetNumber = 0;
        for (auto& planet : planets)
        {
            model = glm::mat4(1.0f);
            distance += 100.0f;
            model = glm::translate(model, glm::vec3((cos(glfwGetTime() / planetSpeedAroundSun[counterForPlanetNumber]) * distance), 0.0f, sin(glfwGetTime() / planetSpeedAroundSun[counterForPlanetNumber]) * distance)); // Move in a circular path
            if (planet == earth && planet == mars && planet == saturn) {
				model = glm::rotate(model, glm::radians(static_cast<float>(glfwGetTime()) * 20.0f), glm::vec3(0.2f, 1.0f, 0.0f)); // Rotate around Y-axis
			}
            else
            {
				model = glm::rotate(model, glm::radians(static_cast<float>(glfwGetTime()) * 20.0f), glm::vec3(0.0f, 1.0f, 0.0f)); // Rotate around Y-axis
			}
            model = glm::scale(model, glm::vec3(4.0f, 4.0f, 4.0f));
            shaderSunLight.set("model", model);
            planet.Draw(shaderSunLight);
            counterForPlanetNumber++;

        }

        
        shaderPlanet.use();
        shaderPlanet.set("projection", projection);
        shaderPlanet.set("view", view);

        model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(0.0f, 0.0f, 0.f));
        model = glm::scale(model, glm::vec3(6.0f, 6.0f, 6.0f));
        shaderPlanet.set("model", model);
        sun.Draw(shaderPlanet);

        glCullFace(GL_FRONT);
        model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(camera.Position));
        model = glm::scale(model, glm::vec3(100.0f, 100.0f, 100.0f));
        shaderPlanet.set("model", model);
        spaceMap.Draw(shaderPlanet);
    
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwTerminate();
    



    return 0;
}
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
}

void processInput(GLFWwindow* window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
    {
        glfwSetWindowShouldClose(window, true);
    }

    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
    {
        camera.ProcessKeyboard(FORWARD, deltaTime);
    }
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
    {
        camera.ProcessKeyboard(BACKWARD, deltaTime);
    }
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
    {
        camera.ProcessKeyboard(LEFT, deltaTime);
    }
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
    {
        camera.ProcessKeyboard(RIGHT, deltaTime);
    }
}

void mouse_callback(GLFWwindow* window, double xposIn, double yposIn)
{

    float xpos = static_cast<float>(xposIn);
    float ypos = static_cast<float>(yposIn);

    if (firstMouse)
    {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }


    float xoffset = xpos - lastX;
    float yoffset = lastY - ypos;
    lastX = xpos;
    lastY = ypos;

    camera.ProcessMouseMovement(xoffset, yoffset);

}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
    camera.ProcessMouseScroll(static_cast<float>(yoffset));
}

void bindModelVAO(Model model)
{
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

void bindDepthMapFBO(unsigned int& depthMapFBO, unsigned int& depthMap)
{
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

void createFPS()
{
    float currentFrame = static_cast<float>(glfwGetTime());
    deltaTime = currentFrame - lastFrame;
    lastFrame = currentFrame;
}

glm::mat4 generateLightSpaceMatrix(glm::vec3 lightPos)
{
    glm::mat4 lightProjection, lightView;
    glm::mat4 lightSpaceMatrix;
    float near_plane = 1.0f, far_plane = 7.5f;
    //lightProjection = glm::perspective(glm::radians(45.0f), (GLfloat)SHADOW_WIDTH / (GLfloat)SHADOW_HEIGHT, near_plane, far_plane); // note that if you use a perspective projection matrix you'll have to change the light position as the current light position isn't enough to reflect the whole scene
    lightProjection = glm::ortho(-10.0f, 10.0f, -10.0f, 10.0f, near_plane, far_plane);
    lightView = glm::lookAt(lightPos, glm::vec3(0.0f), glm::vec3(0.0, 1.0, 0.0));
    lightSpaceMatrix = lightProjection * lightView;
    return lightSpaceMatrix;
}
