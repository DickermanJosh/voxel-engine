#include "CameraController.hpp"

CameraController::CameraController(Camera& camera)
    : m_Camera(camera), m_LastX(400), m_LastY(300), m_FirstMouse(true) {
    std::fill(m_Keys, m_Keys + 1024, false);
}

void CameraController::onKeyPress(sf::Keyboard::Key key) {
    if (key >= 0 && key < 1024)
        m_Keys[key] = true;
}

void CameraController::onKeyRelease(sf::Keyboard::Key key) {
    if (key >= 0 && key < 1024)
        m_Keys[key] = false;
}

void CameraController::onMouseMove(float xcenter, float ycenter, float xoffset, float yoffset) {
    if (m_FirstMouse) {
        m_LastX = xcenter;
        m_LastY = ycenter;
        m_FirstMouse = false;
    }

    float xoffsetDelta = xoffset - xcenter;
    float yoffsetDelta = ycenter - yoffset; // Reversed since y-coordinates go from bottom to top

    m_Camera.processMouseMovement(xoffsetDelta, yoffsetDelta);
}

void CameraController::update(float deltaTime) {
    processKeyboardInternal(deltaTime);
}

void CameraController::processKeyboardInternal(float deltaTime) {
    m_Camera.processKeyboard(m_Keys, deltaTime);
}

