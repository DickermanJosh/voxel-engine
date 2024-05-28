#ifndef SHADER_PROGRAM_HPP
#define SHADER_PROGRAM_HPP

#include <string>
#include <GL/glew.h>

class ShaderProgram {
public:
    // vertex & fragment path are file paths to shader files
    ShaderProgram(const std::string& vertexPath, const std::string& fragmentPath);
    ~ShaderProgram();
    void use();
    GLuint getProgram() const;

private:
    GLuint program;
    GLuint compileShader(GLenum type, const std::string& source);
    std::string readFile(const std::string& filePath);
};

#endif // SHADER_PROGRAM_HPP
