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
#include "Block.hpp"
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
    glFrontFace(GL_CW); // Count-clockwise winding order for front faces

    ShaderProgram shaderProgram("../res/shaders/vertex.glsl", "../res/shaders/fragment.glsl");

    // Floor vertices and indices
    std::vector<float> floorVertices = {
        // positions
        -1.0f,  0.0f, -1.0f,
        -1.0f,  0.0f,  1.0f,
        1.0f,  0.0f,  1.0f,
        1.0f,  0.0f, -1.0f
    };
    std::vector<unsigned int> floorIndices = {
        0, 1, 2, 2, 3, 0
    };

    // Ceiling vertices and indices
    std::vector<float> ceilingVertices = {
        // positions
        -1.0f,  1.0f, -1.0f,
        1.0f,  1.0f, -1.0f,
        1.0f,  1.0f,  1.0f,
        -1.0f,  1.0f,  1.0f
    };
    std::vector<unsigned int> ceilingIndices = {
        0, 1, 2, 2, 3, 0
    };

    // Wall vertices and indices (Negative Z)
    std::vector<float> negWallZVertices = {
        // positions
        -1.0f,  0.0f, -1.0f, // bottom-left
        1.0f,  0.0f, -1.0f, // bottom-right
        1.0f,  1.0f, -1.0f, // top-right
        -1.0f,  1.0f, -1.0f // top-left
    };
    std::vector<unsigned int> negWallZIndices = {
        0, 1, 2, 2, 3, 0
    };

    // Wall vertices and indices (Positive Z)
    std::vector<float> posWallZVertices = {
        // positions
        -1.0f,  0.0f,  1.0f,
        -1.0f,  1.0f,  1.0f,
        1.0f,  1.0f,  1.0f,
        1.0f,  0.0f,  1.0f,
    };
    std::vector<unsigned int> posWallZIndices = {
        0, 1, 2, 2, 3, 0
    };

    // Wall vertices and indices (Negative X)
    std::vector<float> negWallXVertices = {
        // positions
        -1.0f,  0.0f, -1.0f,
        -1.0f,  1.0f, -1.0f,
        -1.0f,  1.0f,  1.0f,
        -1.0f,  0.0f,  1.0f,
    };
    std::vector<unsigned int> negWallXIndices = {
        0, 1, 2, 2, 3, 0
    };

    // Wall vertices and indices (Positive X)
    std::vector<float> posWallXVertices = {
        // positions
        1.0f,  0.0f, -1.0f,
        1.0f,  0.0f,  1.0f,
        1.0f,  1.0f,  1.0f,
        1.0f,  1.0f, -1.0f,
    };
    std::vector<unsigned int> posWallXIndices = {
        0, 1, 2, 2, 3, 0
    };

    Mesh floor(floorVertices, floorIndices);
    Mesh ceiling(ceilingVertices, ceilingIndices);
    Mesh posWallZ(posWallZVertices, posWallZIndices);
    Mesh negWallZ(negWallZVertices, negWallZIndices);
    Mesh posWallX(posWallXVertices, posWallXIndices);
    Mesh negWallX(negWallXVertices, negWallXIndices);
    // Block block(glm::vec3(0.0f, 2.0f, 15.0f));

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
        // Floor - Red
        shaderProgram.setUniform("color", glm::vec4(1.0f, 0.0f, 0.0f, 1.0f));
        floor.draw();

        // Render the ceiling
        glm::mat4 ceilingTransform = glm::mat4(1.0f);
        shaderProgram.setUniform("transform", ceilingTransform);
        // Ceiling - White
        shaderProgram.setUniform("color", glm::vec4(1.0f, 1.0f, 1.0f, 1.0f));
        ceiling.draw();

        // Render the wall in the positive Z direction
        glm::mat4 posWallZTransform = glm::mat4(1.0f);
        shaderProgram.setUniform("transform", posWallZTransform);
        // Positive Z - Yellow
        shaderProgram.setUniform("color", glm::vec4(1.0f, 1.0f, 0.0f, 1.0f));
        posWallZ.draw();

        // Render the wall in the negative Z direction
        glm::mat4 wallZTransform = glm::mat4(1.0f);
        shaderProgram.setUniform("transform", wallZTransform);
        // Negative Z - Green
        shaderProgram.setUniform("color", glm::vec4(0.0f, 1.0f, 0.0f, 1.0f));
        negWallZ.draw();

        // Render the wall in the positive X direction
        glm::mat4 posWallXTransform = glm::mat4(1.0f);
        shaderProgram.setUniform("transform", posWallXTransform);
        // Positive X - Purple
        shaderProgram.setUniform("color", glm::vec4(1.0f, 0.0f, 1.0f, 1.0f));
        posWallX.draw();

        // Render the wall in the negative X direction
        glm::mat4 wallXTransform = glm::mat4(1.0f);
        shaderProgram.setUniform("transform", wallXTransform);
        // Negative X - Blue
        shaderProgram.setUniform("color", glm::vec4(0.0f, 0.0f, 1.0f, 1.0f));
        negWallX.draw();


        // glm::mat4 transform = glm::mat4(1.0f);
        // shaderProgram.setUniform("projection", projection);
        // shaderProgram.setUniform("view", view);
        // shaderProgram.setUniform("transform", transform);
        // shaderProgram.setUniform("color", glm::vec4(1.0f, 1.0f, 1.0f, 1.0f));
        // block.draw();
        // Center the mouse cursor
        sf::Mouse::setPosition(center, window);

        window.display();

        // std::cout << "Camera Pos: " << glm::to_string(camera.getPosition()) << std::endl;
    }

    return 0;
}
