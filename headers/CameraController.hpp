#ifndef CAMERACONTROLLER_HPP
#define CAMERACONTROLLER_HPP

#include "Camera.hpp"
#include "EventListener.hpp"
#include <SFML/Window.hpp>

class CameraController : public EventListener {
public:
    CameraController(Camera& camera);

    void onKeyPress(sf::Keyboard::Key key) override;
    void onKeyRelease(sf::Keyboard::Key key) override;
    void onMouseMove(float xcenter, float ycenter, float xoffset, float yoffset) override;

    void update(float deltaTime);

private:
    Camera& camera;
    bool keys[1024];
    float lastX;
    float lastY;
    bool firstMouse;

    void processKeyboard(float deltaTime);
};

#endif // CAMERACONTROLLER_HPP
