#ifndef APPLICATION_HPP
#define APPLICATION_HPP

#define GLM_ENABLE_EXPERIMENTAL

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/string_cast.hpp>

#include <GL/glew.h>

#include <SFML/OpenGL.hpp>
#include <SFML/Window.hpp>
#include <SFML/Graphics.hpp>
#include <SFML/System/Clock.hpp>

#include "Chunk.hpp"
#include "ShaderProgram.hpp"
#include "Camera.hpp"
#include "CameraController.hpp"
#include "EventHandler.hpp"

#include <memory>

class Application {
    public:
        void run();

        Application();
        ~Application();
    private:
        sf::Clock m_Clock;
        sf::ContextSettings m_Settings;
        sf::RenderWindow m_Window;
        sf::Font m_Font;
        sf::Text m_fpsText;
        sf::Text m_PosText;

        float m_LastFrame = 0.0f;
        float m_FpsTimer = 0.0f;
        unsigned int m_FrameCount = 0;
        float m_CurrentFPS = 0.0f;

        std::unique_ptr<ShaderProgram> m_ShaderProgram;
        GLuint m_TextureAtlas;

        std::unique_ptr<Chunk> m_Chunk; // TODO: Replace with world
        std::unique_ptr<Camera> m_Camera;
        std::unique_ptr<CameraController> m_CameraController;
        EventHandler m_EventHandler;
    private:
        void processEvents();
        void update(float dt);
        void updateOverlay(float dt);
        void render(float dt);
};

#endif // APPLICATION_HPP
