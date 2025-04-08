#include "Camera.hpp"

Camera::Camera(glm::vec3 position, glm::vec3 up, float yaw, float pitch)
    : position(position), worldUp(up), yaw(yaw), pitch(pitch),
      movementSpeed(2.5f), mouseSensitivity(0.1f) {
    updateCameraVectors();
}

Camera::~Camera() {
    // TODO
}

glm::mat4 Camera::getViewMatrix() const {
    return glm::lookAt(position, position + front, up);
}

glm::mat4 Camera::getProjectionMatrix(float aspectRatio) const {
    return glm::perspective(glm::radians(45.0f), aspectRatio, 0.1f, 100.0f);
}

void Camera::processKeyboard(bool* keys, float deltaTime) {
    float velocity = static_cast<float>(movementSpeed * deltaTime);
    if (keys[sf::Keyboard::W])
        position += front * velocity;
    if (keys[sf::Keyboard::S])
        position -= front * velocity;
    if (keys[sf::Keyboard::A])
        position -= right * velocity;
    if (keys[sf::Keyboard::D])
        position += right * velocity;
}

void Camera::processMouseMovement(float xoffset, float yoffset) {
    xoffset *= mouseSensitivity * 0.1;
    yoffset *= mouseSensitivity * 0.1;

    yaw += xoffset;
    pitch += yoffset;

    // clamp camera rotation
    if (pitch > 89.0f)
        pitch = 89.0f;
    if (pitch < -89.0f)
        pitch = -89.0f;

    updateCameraVectors();
}

glm::vec3 Camera::getPosition() const {
    return position;
}

void Camera::updateCameraVectors() {
    glm::vec3 front;
    front.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
    front.y = sin(glm::radians(pitch));
    front.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
    this->front = glm::normalize(front);
    this->right = glm::normalize(glm::cross(this->front, this->worldUp));
    this->up = glm::normalize(glm::cross(this->right, this->front));
}

