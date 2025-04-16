#ifndef PLAYER_HPP
#define PLAYER_HPP

#include <glm/glm.hpp>

#include "Camera.hpp"
#include "CameraController.hpp"
#include "EventHandler.hpp"

class Player {
    public:
        glm::vec3 getPosition();
        EventHandler* getEventHandler();
        Camera* getCamera();
        void update(float dt);

        Player(const glm::vec3& position);
        // ~Player();

    private:
        glm::vec3 m_Position;
        Camera m_Camera;
        CameraController m_CameraController;
        EventHandler m_EventHandler;

        // Mesh m_Mesh;
        // MeshPack m_MeshPack;
};

#endif // PLAYER_HPP

