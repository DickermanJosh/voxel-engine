#ifndef SHADER_PROGRAM_HPP
#define SHADER_PROGRAM_HPP

#include <string>
#include <GL/glew.h>
#include <glm/glm.hpp>

class ShaderProgram {
    public:
        void use();
        void setUniform(const std::string& name, const glm::mat4& matrix);
        void setUniform(const std::string& name, const glm::vec4& value);
        GLuint getProgram() const;

        // vertex & fragment path are file paths to shader files
        ShaderProgram(const std::string& vertexPath, const std::string& fragmentPath);
        ~ShaderProgram();

    private:
        GLuint m_Program;
    private:
        GLuint compileShader(GLenum type, const std::string& source);
        std::string readFile(const std::string& filePath);
};

#endif // SHADER_PROGRAM_HPP
