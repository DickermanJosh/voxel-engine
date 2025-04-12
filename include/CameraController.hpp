#ifndef CAMERACONTROLLER_HPP
#define CAMERACONTROLLER_HPP

#include "Camera.hpp"
#include "EventListener.hpp"
#include <SFML/Window.hpp>

class CameraController : public EventListener {
    public:
        void onKeyPress(sf::Keyboard::Key key) override;
        void onKeyRelease(sf::Keyboard::Key key) override;
        void onMouseMove(float xcenter, float ycenter, float xoffset, float yoffset) override;
        void update(float deltaTime);

        CameraController(Camera& camera);

    private:
        Camera& m_Camera;
        bool m_Keys[1024];
        float m_LastX;
        float m_LastY;
        bool m_FirstMouse;
    private:
        void processKeyboardInternal(float deltaTime);
};

#endif // CAMERACONTROLLER_HPP
