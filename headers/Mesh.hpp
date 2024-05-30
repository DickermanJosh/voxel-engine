// #ifndef MESH_HPP
#define MESH_HPP

#include <GL/glew.h>
#include <vector>

class Mesh {
    public:
        Mesh(const std::vector<float>& vertices, const std::vector<unsigned int>& indices);
        ~Mesh();
        void draw() const;
        void setupMesh();

    private:
        unsigned int VAO, VBO, EBO;
        std::vector<float> vertices;
        std::vector<unsigned int> indices;
};
