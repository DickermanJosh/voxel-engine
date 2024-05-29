#include <GL/glew.h>
#include <SFML/OpenGL.hpp>
#include <SFML/Window.hpp>
#include <glm/glm.hpp>
#include <iostream>

#include "ShaderProgram.hpp"
#include "Mesh.hpp"

int main() {
  sf::ContextSettings settings;
    settings.depthBits = 24;
    settings.stencilBits = 8;
    settings.antialiasingLevel = 4;
    settings.majorVersion = 4;
    settings.minorVersion = 6;

    sf::Window window(sf::VideoMode(2560, 1440), "Minecraft", sf::Style::Default, settings);
    window.setActive(true);

    GLenum err = glewInit();
    if (err != GLEW_OK) {
        std::cerr << "Failed to init GLEW: " << glewGetErrorString(err) << std::endl;
        return -1;
    }

    ShaderProgram shaderProgram("../res/shaders/vertex.glsl", "../res/shaders/fragment.glsl");

    // Vertices and indices for a simple square
    std::vector<float> vertices = {
        0.5f,  0.5f, 0.0f,  // top right
        0.5f, -0.5f, 0.0f,  // bottom right
        -0.5f, -0.5f, 0.0f,  // bottom left
        -0.5f,  0.5f, 0.0f   // top left 
    };
    std::vector<unsigned int> indices = {  
        0, 1, 3,   // first triangle
        1, 2, 3    // second triangle
    };

    Mesh mesh(vertices, indices);

    glm::mat4 transform = glm::mat4(1.0f);
    glm::vec4 color(0.2f, 1.0f, 0.0f, 1.0f);

    while (window.isOpen()) {
        glClearColor(0.0f, 0.3f, 0.6f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        shaderProgram.use();
        shaderProgram.setUniform("transform", transform);
        shaderProgram.setUniform("color", color);
        
        mesh.draw();
        
        window.display();
    }

    return 0;
}
