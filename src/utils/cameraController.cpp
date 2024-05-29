#include <iostream>
#include "CameraController.hpp"

CameraController::CameraController(Camera& camera)
    : camera(camera), lastX(400), lastY(300), firstMouse(true) {
    std::fill(keys, keys + 1024, false);
}

void CameraController::onKeyPress(sf::Keyboard::Key key) {
    if (key >= 0 && key < 1024)
        keys[key] = true;
}

void CameraController::onKeyRelease(sf::Keyboard::Key key) {
    if (key >= 0 && key < 1024)
        keys[key] = false;
}

void CameraController::onMouseMove(float xcenter, float ycenter, float xoffset, float yoffset) {
    if (firstMouse) {
        lastX = xcenter;
        lastY = ycenter;
        firstMouse = false;
    }

    float xoffsetDelta = xoffset - xcenter;
    float yoffsetDelta = ycenter - yoffset; // Reversed since y-coordinates go from bottom to top

    camera.processMouseMovement(xoffsetDelta, yoffsetDelta);
}

void CameraController::update(float deltaTime) {
    processKeyboard(deltaTime);
}

void CameraController::processKeyboard(float deltaTime) {
    camera.processKeyboard(keys, deltaTime);
}

