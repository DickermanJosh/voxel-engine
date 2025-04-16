#include "Camera.hpp"

Camera::Camera(glm::vec3 position)
    : m_Position(position), m_WorldUp(glm::vec3(0.0f, 1.0f, 0.0f)), m_Yaw(-90.0f), m_Pitch(0.0f),
      m_MovementSpeed(BASE_MOVE_SPEED), m_MouseSensitivity(0.1f) {
    updateCameraVectorsInternal();
}

Camera::Camera(glm::vec3 position, glm::vec3 up, float yaw, float pitch)
    : m_Position(position), m_WorldUp(up), m_Yaw(yaw), m_Pitch(pitch),
      m_MovementSpeed(BASE_MOVE_SPEED), m_MouseSensitivity(0.1f) {
    updateCameraVectorsInternal();
}

Camera::~Camera() {
    // TODO
}

glm::vec3 Camera::getPosition() const {
    return m_Position;
}

glm::mat4 Camera::getViewMatrix() const {
    return glm::lookAt(m_Position, m_Position + m_Front, m_Up);
}

glm::mat4 Camera::getProjectionMatrix(float aspectRatio) const {
    return glm::perspective(glm::radians(45.0f), aspectRatio, 0.1f, 100.0f);
}

void Camera::processKeyboard(bool* keys, float deltaTime) {
    float velocity = static_cast<float>(m_MovementSpeed * deltaTime);
    if (keys[sf::Keyboard::W]) {
        m_Position += m_Front * velocity;
    }

    if (keys[sf::Keyboard::A]) {
        m_Position -= m_Right * velocity;
    }

    if (keys[sf::Keyboard::S]) {
        m_Position -= m_Front * velocity;
    }

    if (keys[sf::Keyboard::D]) {
        m_Position += m_Right * velocity;
    }

    if (keys[sf::Keyboard::Space]) {
        m_Position += m_Up * velocity;
    }

    if (keys[sf::Keyboard::LShift]) {
        m_Position -= m_Up * velocity;
    }

    if (keys[sf::Keyboard::LControl]) {
        m_MovementSpeed = SPRINT_MOVE_SPEED;
    } else {
        m_MovementSpeed = BASE_MOVE_SPEED;
    }
}

void Camera::processMouseMovement(float xoffset, float yoffset) {
    xoffset *= m_MouseSensitivity * 0.1;
    yoffset *= m_MouseSensitivity * 0.1;

    m_Yaw += xoffset;
    m_Pitch += yoffset;

    // clamp camera rotation
    if (m_Pitch > 89.0f)
        m_Pitch = 89.0f;
    if (m_Pitch < -89.0f)
        m_Pitch = -89.0f;

    updateCameraVectorsInternal();
}

void Camera::updateCameraVectorsInternal() {
    glm::vec3 front;
    front.x = cos(glm::radians(m_Yaw)) * cos(glm::radians(m_Pitch));
    front.y = sin(glm::radians(m_Pitch));
    front.z = sin(glm::radians(m_Yaw)) * cos(glm::radians(m_Pitch));
    this->m_Front = glm::normalize(front);
    this->m_Right = glm::normalize(glm::cross(this->m_Front, this->m_WorldUp));
    this->m_Up = glm::normalize(glm::cross(this->m_Right, this->m_Front));
}

