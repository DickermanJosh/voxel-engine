#ifndef MESH_HPP
#define MESH_HPP

#include <GL/glew.h>
#include <MeshPack.hpp>

struct Mesh {
    public:
        void draw() const;
        void setupMesh();

        Mesh(const MeshPack& pack);
        ~Mesh();

    private:
        unsigned int m_VAO, m_VBO, m_EBO;
        MeshPack m_MeshPack;
};

#endif // MESH_HPP
