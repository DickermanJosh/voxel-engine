#define GLM_ENABLE_EXPERIMENTAL
#include <GL/glew.h>
#include <SFML/OpenGL.hpp>
#include <SFML/Window.hpp>
#include <SFML/System/Clock.hpp>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/string_cast.hpp>
#include <iostream>

#include "ShaderProgram.hpp"
#include "Mesh.hpp"
#include "Camera.hpp"
#include "CameraController.hpp"
#include "EventHandler.hpp"

int main() {
  sf::ContextSettings settings;
    settings.depthBits = 24;
    settings.stencilBits = 8;
    settings.antialiasingLevel = 4;
    settings.majorVersion = 4;
    settings.minorVersion = 6;

    sf::Window window(sf::VideoMode(2560, 1440), "Minecraft", sf::Style::Default, settings);
    window.setActive(true);
    window.setMouseCursorVisible(false);

    GLenum err = glewInit();
    if (err != GLEW_OK) {
        std::cerr << "Failed to init GLEW: " << glewGetErrorString(err) << std::endl;
        return -1;
    }

    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL);

    ShaderProgram shaderProgram("../res/shaders/vertex.glsl", "../res/shaders/fragment.glsl");

    // Floor vertices and indices
    std::vector<float> floorVertices = {
        // positions
        -5.0f,  0.0f, -5.0f,
         5.0f,  0.0f, -5.0f,
         5.0f,  0.0f,  5.0f,
        -5.0f,  0.0f,  5.0f
    };
    std::vector<unsigned int> floorIndices = {
        0, 1, 2, 2, 3, 0
    };

    // Wall vertices and indices (Negative Z)
    std::vector<float> wallZVertices = {
        // positions
        -5.0f,  0.0f, -5.0f,
         5.0f,  0.0f, -5.0f,
         5.0f,  5.0f, -5.0f,
        -5.0f,  5.0f, -5.0f
    };
    std::vector<unsigned int> wallZIndices = {
        0, 1, 2, 2, 3, 0
    };

    // Wall vertices and indices (Negative X)
    std::vector<float> wallXVertices = {
        // positions
        -5.0f,  0.0f, -5.0f,
        -5.0f,  0.0f,  5.0f,
        -5.0f,  5.0f,  5.0f,
        -5.0f,  5.0f, -5.0f
    };
    std::vector<unsigned int> wallXIndices = {
        0, 1, 2, 2, 3, 0
    };

    Mesh floor(floorVertices, floorIndices);
    Mesh wallZ(wallZVertices, wallZIndices);
    Mesh wallX(wallXVertices, wallXIndices);

    // Camera Setup
    glm::vec3 cameraPos = glm::vec3(0.0f, 1.0f, 3.0f);
    glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);
    Camera camera(cameraPos, cameraUp, -90.0f, 0.0f);
    CameraController cameraController(camera);

    // Event Handler, currently only set up to handle camera inputs
    EventHandler eventHandler;
    eventHandler.addEventListener(&cameraController);

    // Clock for keeping track of deltatime. TODO: Cap at 144fps
    sf::Clock clock;
    float lastFrame = 0.0f;

    while (window.isOpen()) {
        float currentFrame = clock.getElapsedTime().asSeconds();
        float deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        // Get current window size and center in case of resize. TODO: Change this to happen in a listener, not just every frame
        sf::Vector2u windowSize = window.getSize();
        sf::Vector2i center(windowSize.x / 2, windowSize.y / 2);

        eventHandler.processEvents(window);
        cameraController.update(deltaTime);

        glClearColor(0.0f, 0.3f, 0.6f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        shaderProgram.use();

        // Get the current aspect ratio for the perspective camera. TODO: Change this to happen in a listener, not just every frame
        float aspectRatio = static_cast<float>(windowSize.x) / static_cast<float>(windowSize.y);
        glm::mat4 projection = camera.getProjectionMatrix(aspectRatio);
        glm::mat4 view = camera.getViewMatrix();

        // Render the floor
        glm::mat4 floorTransform = glm::mat4(1.0f);
        shaderProgram.setUniform("projection", projection);
        shaderProgram.setUniform("view", view);
        shaderProgram.setUniform("transform", floorTransform);
        shaderProgram.setUniform("color", glm::vec4(1.0f, 0.0f, 0.0f, 1.0f));
        floor.draw();

        // Render the wall in the negative Z direction
        glm::mat4 wallZTransform = glm::mat4(1.0f);
        shaderProgram.setUniform("transform", wallZTransform);
        shaderProgram.setUniform("color", glm::vec4(0.0f, 1.0f, 0.0f, 1.0f));
        wallZ.draw();

        // Render the wall in the negative X direction
        glm::mat4 wallXTransform = glm::mat4(1.0f);
        shaderProgram.setUniform("transform", wallXTransform);
        shaderProgram.setUniform("color", glm::vec4(0.0f, 0.0f, 1.0f, 1.0f));
        wallX.draw();

        // Center the mouse cursor
        sf::Mouse::setPosition(center, window);

        window.display();

        std::cout << "Camera Pos: " << glm::to_string(camera.getPosition()) << std::endl;
    }

    return 0;
}
