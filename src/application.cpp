#include "Application.hpp"
#include "Utils.hpp"
#include <sstream>
#include <iomanip>
#include <iostream>

Application::Application() 
    : m_Settings(24, 8, 4, 4, 6),
    m_Window(sf::VideoMode(2560, 1440), "Minecraft", sf::Style::Default, m_Settings) {
    /*m_Settings.depthBits = 24;
    m_Settings.stencilBits = 8;
    m_Settings.antialiasingLevel = 4;
    m_Settings.majorVersion = 4;
    m_Settings.minorVersion = 6;*/

    m_Window.setActive(true);
    m_Window.setMouseCursorVisible(false);
    m_Window.setVerticalSyncEnabled(true);

    GLenum err = glewInit();
    if (err != GLEW_OK) {
        std::cout << "Failed to init GLEW: " << glewGetErrorString(err) << std::endl;
        throw std::runtime_error("Failed to load GLEW.");
    }

    std::filesystem::path basePath = getExecutableDir();
    std::filesystem::path fontPath = basePath / "../res/fonts/SpecialGothicCondensedOne-Regular.ttf";

    if (!m_Font.loadFromFile(fontPath.string())) {
        std::cout <<"Failed to load ttf font file" << std::endl;
        throw std::runtime_error("Failed to load font.");
        
    }

    m_fpsText.setFont(m_Font);
    m_fpsText.setCharacterSize(28);
    m_fpsText.setFillColor(sf::Color::White);
    m_fpsText.setPosition(10.f, 10.f);

    m_PosText.setFont(m_Font);
    m_PosText.setCharacterSize(28);
    m_PosText.setFillColor(sf::Color::White);
    m_PosText.setPosition(10.f, 30.f); // Slightly below the FPS

    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL);

    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    glFrontFace(GL_CW); // Counter-clockwise winding order for front faces

    std::filesystem::path vertexPath = basePath / "../res/shaders/vertex.glsl";
    std::filesystem::path fragmentPath = basePath / "../res/shaders/fragment.glsl";

    m_ShaderProgram = std::make_unique<ShaderProgram>(vertexPath.string(), fragmentPath.string());

    std::filesystem::path atlasPath = basePath / "../res/blocks.png";

    m_TextureAtlas = loadTexture(atlasPath.string().c_str());
    m_ShaderProgram->use();
    m_ShaderProgram->setUniform("textureAtlas", 0);

    // GrassBlock block(glm::vec3(0.0f, 0.0f, 0.0f), std::vector<bool>(6, true));
    // StoneBlock stoneBlock(glm::vec3(0.0f, 2.0f, 0.0f), std::vector<bool>(6, true));
    uint64_t rand_seed = generate_uint64_t();
    m_World = std::make_unique<World>(rand_seed);

    // Camera Setup
    glm::vec3 cameraPos = glm::vec3(0.0f, 1.0f, 3.0f);
    glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);
    m_Camera = std::make_unique<Camera>(cameraPos, cameraUp, -90.0f, 0.0f);
    m_CameraController = std::make_unique<CameraController>(*m_Camera);

    // Event Handler, currently only set up to handle camera inputs
    m_EventHandler.addEventListener(m_CameraController.get());
}

Application::~Application() {
    // TODO: Clean up stored world, camera and window
}

void Application::run() {
    std::cout << "[Application] run() starting..." << std::endl;

    m_World->update(*m_Camera.get(), 0.0f);
    while (m_Window.isOpen()) {
        float currentFrame = m_Clock.getElapsedTime().asSeconds();
        float deltaTime = currentFrame - m_LastFrame;
        m_LastFrame = currentFrame;

        processEvents();
        update(deltaTime);
        render(deltaTime);
    }
}

void Application::processEvents() {
    sf::Vector2u size = m_Window.getSize();
    sf::Vector2i center(size.x / 2, size.y / 2);
    m_EventHandler.processEvents(m_Window);
    sf::Mouse::setPosition(center, m_Window);
}

void Application::update(float deltaTime) {
    m_CameraController->update(deltaTime);
    //m_World->update(*m_Camera.get(), deltaTime);
}

void Application::render(float deltaTime) {
    m_Window.clear();
    glClearColor(0.0f, 0.3f, 0.6f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    m_ShaderProgram->use();

    float aspect = static_cast<float>(m_Window.getSize().x) / m_Window.getSize().y;
    glm::mat4 projection = m_Camera->getProjectionMatrix(aspect);
    glm::mat4 view = m_Camera->getViewMatrix();

    m_ShaderProgram->setUniform("projection", projection);
    m_ShaderProgram->setUniform("view", view);
    m_ShaderProgram->setUniform("transform", glm::mat4(1.0f));

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, m_TextureAtlas);

    m_World->draw();

    updateOverlay(deltaTime);

    sf::RenderWindow* renderWindow = static_cast<sf::RenderWindow*>(&m_Window);
    m_Window.pushGLStates();
    
    renderWindow->draw(m_fpsText);
    renderWindow->draw(m_PosText);

    m_Window.popGLStates();

    m_Window.display();
}

void Application::updateOverlay(float deltaTime) {
    m_FrameCount++;
    m_FpsTimer += deltaTime;
    if (m_FpsTimer >= 0.1f) {
        m_CurrentFPS = static_cast<float>(m_FrameCount) / m_FpsTimer;
        m_fpsText.setString("FPS: " + std::to_string(static_cast<int>(m_CurrentFPS)));

        glm::vec3 pos = m_Camera->getPosition();
        std::ostringstream oss;
        oss << std::fixed << std::setprecision(2)
            << "\nx: " << pos.x
            << "\nY: " << pos.y
            << "\nZ: " << pos.z;
        m_PosText.setString(oss.str());

        m_FpsTimer = 0.0f;
        m_FrameCount = 0;
    }
}
