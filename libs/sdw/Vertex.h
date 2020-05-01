#ifndef VERTEX_H
#define VERTEX_H

#include "Header.h"

class Vertex {
    public:
        glm::vec3 position;
        std::vector<int> triangles;
        glm::vec3 vert_normal;

    Vertex() {

    }

    Vertex(glm::vec3 position){
        this->position = position;
    }
    
};

#endif