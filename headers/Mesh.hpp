// #ifndef MESH_HPP
#define MESH_HPP

#include <GL/glew.h>
#include <vector>

class Mesh {
    public:
        Mesh(const std::vector<float>& vertices, const std::vector<unsigned int>& indices);
        ~Mesh();
        void draw() const;

    private:
        void setupMesh();

        unsigned int VAO, VBO, EBO;
        std::vector<float> vertices;
        std::vector<unsigned int> indices;
};
