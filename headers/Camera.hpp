#ifndef CAMERA_HPP
#define CAMERA_HPP

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <SFML/Window.hpp>

class Camera {
    public:
        glm::vec3 getPosition() const;
        glm::mat4 getViewMatrix() const;
        glm::mat4 getProjectionMatrix(float aspectRatio) const;
        void processKeyboard(bool* keys, float deltaTime);
        void processMouseMovement(float xoffset, float yoffset);

        Camera(glm::vec3 position, glm::vec3 up, float yaw, float pitch);
        ~Camera();

    private:
        glm::vec3 m_Position;
        glm::vec3 m_Front;
        glm::vec3 m_Up;
        glm::vec3 m_Right;
        glm::vec3 m_WorldUp;

        float m_Yaw;
        float m_Pitch;

        float m_MovementSpeed;
        float m_MouseSensitivity;
    private:
        void updateCameraVectorsInternal();
};

#endif // CAMERA_HPP
