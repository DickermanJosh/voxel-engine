#include "EventHandler.hpp"
#include <GL/gl.h>

void EventHandler::addEventListener(EventListener* listener) {
    m_Listeners.push_back(listener);
}

void EventHandler::processEvents(sf::Window& window) {
    sf::Event event;
    while (window.pollEvent(event)) {
        switch (event.type) {
            case sf::Event::KeyPressed:
                for (auto listener : m_Listeners)
                    listener->onKeyPress(event.key.code);
                break;
            case sf::Event::KeyReleased:
                for (auto listener : m_Listeners)
                    listener->onKeyRelease(event.key.code);
                break;
            case sf::Event::MouseMoved:
                for (auto listener : m_Listeners) {
                    sf::Vector2u windowSize = window.getSize();
                    sf::Vector2i center(windowSize.x / 2, windowSize.y / 2);
                    listener->onMouseMove(center.x, center.y, event.mouseMove.x, event.mouseMove.y);
                }
                break;
            case sf::Event::Resized:
                glViewport(0, 0, event.size.width, event.size.height);
                break;
            case sf::Event::Closed:
                window.close();
                break;
            default:
                break;
        }
    }
}
