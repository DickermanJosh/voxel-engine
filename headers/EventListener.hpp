#ifndef EVENTLISTENER_HPP
#define EVENTLISTENER_HPP

#include <SFML/Window.hpp>

class EventListener {
    public:
        virtual void onKeyPress(sf::Keyboard::Key key) = 0;
        virtual void onKeyRelease(sf::Keyboard::Key key) = 0;
        virtual void onMouseMove(float xcenter, float ycenter, float xoffset, float yoffset) = 0;
};

#endif // EVENTLISTENER_HPP
