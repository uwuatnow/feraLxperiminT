#pragma once
#include <vector>
#include <string>
#include <SFML/OpenGL.hpp>

namespace nyaa {

struct Vertex {
    float x, y, z;
    float nx, ny, nz;
    float u, v;
};

class Model {
public:
    std::vector<Vertex> vertices;
    std::vector<unsigned int> indices;

    void draw() const {
        if (vertices.empty()) return;

        glBegin(GL_TRIANGLES);
        for (size_t i = 0; i < vertices.size(); ++i) {
            const Vertex& v = vertices[i];
            glNormal3f(v.nx, v.ny, v.nz);
            glTexCoord2f(v.u, v.v);
            glVertex3f(v.x, v.y, v.z);
        }
        glEnd();
    }
    
    void clear() {
        vertices.clear();
        indices.clear();
    }
};

}
