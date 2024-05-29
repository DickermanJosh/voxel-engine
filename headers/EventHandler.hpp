#ifndef EVENTHANDLER_HPP
#define EVENTHANDLER_HPP

#include <SFML/Window.hpp>
#include "EventListener.hpp"

class EventHandler {
public:
    void addEventListener(EventListener* listener);

    void processEvents(sf::Window& window);

private:
    std::vector<EventListener*> listeners;
};

#endif // EVENTHANDLER_HPP
