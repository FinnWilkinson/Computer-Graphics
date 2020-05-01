#ifndef MODELTRIANGLE_H
#define MODELTRIANGLE_H

#include "Header.h"

class ModelTriangle
{
    public:
      glm::vec3 vertices[3];
      TexturePoint texture_points[3];
      glm::vec3 normal;
      glm::vec3 vertex_normals[3];
      Colour colour;
      bool textured = false;
      bool visible = true;

      ModelTriangle()
      {
      }

      ModelTriangle(glm::vec3 v0, glm::vec3 v1, glm::vec3 v2)
      {
        vertices[0] = v0;
        vertices[1] = v1;
        vertices[2] = v2;
        colour = Colour(255, 255, 255);
        normal = glm::normalize(glm::cross((v1 - v0), (v2 - v0)));
      }

      ModelTriangle(glm::vec3 v0, glm::vec3 v1, glm::vec3 v2, Colour triColour)
      {
        vertices[0] = v0;
        vertices[1] = v1;
        vertices[2] = v2;
        colour = triColour;
        normal = glm::normalize(glm::cross((v1 - v0), (v2 - v0)));
        if(colour.specular) std::cout << normal.x << ", " << normal.y << ", " << normal.z << std::endl;
      }

      ModelTriangle(glm::vec3 v0, glm::vec3 v1, glm::vec3 v2, Colour triColour, TexturePoint t0, TexturePoint t1, TexturePoint t2)
      {
        vertices[0] = v0;
        vertices[1] = v1;
        vertices[2] = v2;
        colour = triColour;
        normal = glm::normalize(glm::cross((v1 - v0), (v2 - v0)));
        texture_points[0] = t0;
        texture_points[1] = t1;
        texture_points[2] = t2;
      }
};

std::ostream& operator<<(std::ostream& os, const ModelTriangle& triangle)
{
    os << "(" << triangle.vertices[0].x << ", " << triangle.vertices[0].y << ", " << triangle.vertices[0].z << ")" << std::endl;
    os << "(" << triangle.vertices[1].x << ", " << triangle.vertices[1].y << ", " << triangle.vertices[1].z << ")" << std::endl;
    os << "(" << triangle.vertices[2].x << ", " << triangle.vertices[2].y << ", " << triangle.vertices[2].z << ")" << std::endl;
    os << std::endl;
    return os;
}

#endif
