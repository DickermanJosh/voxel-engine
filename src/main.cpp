#define GLM_ENABLE_EXPERIMENTAL
#include <GL/glew.h>
#include <SFML/OpenGL.hpp>
#include <SFML/Window.hpp>
#include <SFML/System/Clock.hpp>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/string_cast.hpp>
#include <iostream>

#include "Utils.hpp"
#include "ShaderProgram.hpp"
#include "GrassBlock.hpp"
#include "StoneBlock.hpp"
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
    window.setVerticalSyncEnabled(true);

    GLenum err = glewInit();
    if (err != GLEW_OK) {
        std::cerr << "Failed to init GLEW: " << glewGetErrorString(err) << std::endl;
        return -1;
    }

    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL);

    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    glFrontFace(GL_CW); // Counter-clockwise winding order for front faces

    ShaderProgram shaderProgram("../res/shaders/vertex.glsl", "../res/shaders/fragment.glsl");

    GLuint textureAtlas = loadTexture("../res/blocks.png");
    shaderProgram.use();
    shaderProgram.setUniform("textureAtlas", 0);

    GrassBlock block(glm::vec3(0.0f, 0.0f, 0.0f), std::vector<bool>(6, true));
    StoneBlock stoneBlock(glm::vec3(0.0f, 2.0f, 0.0f), std::vector<bool>(6, true));

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

        glm::mat4 blockTransform = glm::mat4(1.0f);
        shaderProgram.setUniform("projection", projection);
        shaderProgram.setUniform("view", view);
        shaderProgram.setUniform("transform", blockTransform);
        block.draw();
        stoneBlock.draw();

        // Center the mouse cursor
        sf::Mouse::setPosition(center, window);

        window.display();

        // std::cout << "Camera Pos: " << glm::to_string(camera.getPosition()) << std::endl;
    }

    return 0;
}
