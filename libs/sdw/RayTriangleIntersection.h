#ifndef RAYTRIANGLEINTERSECTION_H
#define RAYTRAINGLEINTERSECTION_H

#include "Header.h"

class RayTriangleIntersection
{
  public:

    CanvasPoint screenPoint;
    glm::vec3 worldPoint;
    float distanceFromCamera;
    glm::vec2 uv;
    ModelTriangle intersectedTriangle;
    Colour ssaaColour;
    int intersectedIndex = -1;
    double incidence;
    bool inShadow;

    RayTriangleIntersection()
    {
      distanceFromCamera = INFINITY;
      inShadow = false;
    }
};

#endif
