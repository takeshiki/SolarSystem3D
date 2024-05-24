#pragma once

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>



enum Camera_Movement
{
	FORWARD,
	BACKWARD,
	LEFT,
	RIGHT
};

const float YAW = -90.0f;
const float PITCH = 0.0f;
const float SPEED = 2.5f;
const float SENSITIVITY = 0.1f;
const float ZOOM = 45.0f;

class Camera
{
public:
    // атрибути камери
    glm::vec3 Position;
    glm::vec3 Front;
    glm::vec3 Up;
    glm::vec3 Right;
    glm::vec3 WorldUp;
    // кути ейлера
    float Yaw;
    float Pitch;
    // параметри камери
    float MovementSpeed;
    float MouseSensitivity;
    float Zoom;

    // конструктор з векторами
    Camera(glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f), float yaw = YAW, float pitch = PITCH);
    // конструктор з скалярами
    Camera(float posX, float posY, float posZ, float upX, float upY, float upZ, float yaw, float pitch);

    // повертає матрицю подиву камери (вигляду) залежно від її поточних параметрів
    glm::mat4 GetViewMatrix();

    // обробка вводу з клавіатури (натискання клавіш) для переміщення камери вперед/назад/ліворуч/праворуч відповідно до напрямку камери (або світу) 
    void ProcessKeyboard(Camera_Movement direction, float deltaTime);

    // обробка вводу з миші для повороту камери вліво/вправо та вгору/вниз 
    void ProcessMouseMovement(float xoffset, float yoffset, GLboolean constrainPitch = true);

    // обробка вводу з миші для зуму (приближення/віддалення) камери
    void ProcessMouseScroll(float yoffset);

private:
    // оновлення векторів камери з новими кутами ейлера 
    void updateCameraVectors();
};