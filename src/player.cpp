#include "Player.hpp"

Player::Player(const glm::vec3& position)
    : m_Position(position),
    m_Camera(position),
    m_CameraController(m_Camera) {
        m_EventHandler.addEventListener(&m_CameraController);
    }

void Player::update(float dt) {
    m_CameraController.update(dt);
    m_Position = m_Camera.getPosition();
}

glm::vec3 Player::getPosition() {
    return m_Position;
}

EventHandler* Player::getEventHandler() {
    return &m_EventHandler;
}

Camera* Player::getCamera() {
    return &m_Camera;
}
