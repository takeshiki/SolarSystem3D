#include "Scene.h"

#include <glm/gtc/type_ptr.hpp>

#include "Shader.h"
#include "Model.h"

#include <iostream>
#include <map>

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow* window);
void mouse_callback(GLFWwindow* window, double xposIn, double yposIn);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void createFPS();

Camera camera(glm::vec3(-200.0f, 200.0f, 800.0f));
float lastX = SCR_WIDTH / 2.0f;
float lastY = SCR_HEIGHT / 2.0f;
bool firstMouse = true;

// timing
float deltaTime = 0.0f;	// час між поточним кадром і попереднім кадром
float lastFrame = 0.0f;

int main(int, char**) {
    
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 4);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_SAMPLES, 4);

    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "thing", NULL, NULL);
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl; // Помилка при створенні вікна GLFW
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
        std::cout << "Failed to initialize GLAD" << std::endl; // Помилка при ініціалізації GLAD
        return -1;
    }

    glEnable(GL_DEPTH_TEST); 
    glEnable(GL_CULL_FACE);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    
    // створення шейдерів
    Shader shaderSunLight("src/glsl/sunlight_vertex.glsl", "src/glsl/sunlight_fragment.glsl");
    Shader shaderShadow("src/glsl/shadow_vertex.glsl", "src/glsl/shadow_fragment.glsl");
    Shader shaderPlanet("src/glsl/planet_vertex.glsl", "src/glsl/planet_fragment.glsl");\

    // створення моделей
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
    Model moon("objects/moon/moon.obj");
    Model saturnCircles("objects/saturn/saturnCircles.obj");
    
    std::vector<Model> planets{ mercury, venus, earth, mars, jupiter, saturn, uranus, neptune};
    std::map<int, Model> spaceObjects{
        {0 , sun },
        {2 , moon},
        {5, saturnCircles},
        {1, spaceMap}
    };
    Scene scene(planets, spaceObjects, shaderPlanet, shaderSunLight, shaderShadow);
   

    while (!glfwWindowShouldClose(window))
    {
        createFPS();

        processInput(window);

        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glCullFace(GL_BACK);

        scene.renderScene(camera.GetViewMatrix(), camera.Position);// опрацювання сцени
        
        glfwSwapBuffers(window);
        glfwPollEvents();
    }
    glfwTerminate();
    return 0;
}
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{// обробка зміни розміру вікна
    glViewport(0, 0, width, height);
}

void processInput(GLFWwindow* window)
{// обробка натискання клавіш
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
{// обробка руху миші

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
{// обробка прокрутки миші
    camera.ProcessMouseScroll(static_cast<float>(yoffset));
}

void createFPS()
{// обрахування часу між кадрами
    float currentFrame = static_cast<float>(glfwGetTime());
    deltaTime = currentFrame - lastFrame;
    lastFrame = currentFrame;
}