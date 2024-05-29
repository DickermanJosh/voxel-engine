#include "EventHandler.hpp"

void EventHandler::addEventListener(EventListener* listener) {
    listeners.push_back(listener);
}

void EventHandler::processEvents(sf::Window& window) {
    sf::Event event;
    while (window.pollEvent(event)) {
        switch (event.type) {
            case sf::Event::KeyPressed:
                for (auto listener : listeners)
                    listener->onKeyPress(event.key.code);
                break;
            case sf::Event::KeyReleased:
                for (auto listener : listeners)
                    listener->onKeyRelease(event.key.code);
                break;
            case sf::Event::MouseMoved:
                for (auto listener : listeners) {
                    sf::Vector2u windowSize = window.getSize();
                    sf::Vector2i center(windowSize.x / 2, windowSize.y / 2);
                    listener->onMouseMove(center.x, center.y, event.mouseMove.x, event.mouseMove.y);
                    // TODO: try centering here instead of inside game loop
                }
                break;
            case sf::Event::Closed:
                window.close();
                break;
            default:
                break;
        }
    }
}
