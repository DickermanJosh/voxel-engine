#include <GL/glew.h>
#include <SFML/OpenGL.hpp>
#include <SFML/Window.hpp>
#include <glm/glm.hpp>
#include <iostream>

int main() {
    sf::ContextSettings settings;
    settings.majorVersion = 4;
    settings.minorVersion = 6;

    // Create SFML window
    sf::Window window(sf::VideoMode(2560, 1440), "Minecraft", sf::Style::Default, settings);
    window.setActive(true);

    // Init GLEW
    GLenum err = glewInit();
    if (err != GLEW_OK) {
        std::cerr << "Failed to init GLEW: " << glewGetErrorString(err) << std::endl;
        return -1;
    }

    while (window.isOpen()) {

        glClearColor(0.0f, 0.0f, 1.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        window.display();
    }

    return 0;
}
