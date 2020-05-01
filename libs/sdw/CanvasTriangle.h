#ifndef CANVASTRIANGLE_H
#define CANVASTRIANGLE_H

#include "Header.h"

class CanvasTriangle
{
  public:
    CanvasPoint vertices[3];
    CanvasPoint middlePoint;
    TexturePoint texture_points[3];
    Texture texture;
    Colour colour;
    bool textured;

    CanvasTriangle()
    {
    }

    CanvasTriangle(CanvasPoint v0, CanvasPoint v1, CanvasPoint v2)
    {
      vertices[0] = v0;
      vertices[1] = v1;
      vertices[2] = v2;
      orderVerts();
      findMiddle();
      colour = Colour(255,255,255);
      textured = false;
    }

    CanvasTriangle(CanvasPoint v0, CanvasPoint v1, CanvasPoint v2, Colour c)
    {
      vertices[0] = v0;
      vertices[1] = v1;
      vertices[2] = v2;
      orderVerts();
      findMiddle();
      textured = false;
      colour = c;
    }

    CanvasTriangle(CanvasPoint v0, CanvasPoint v1, CanvasPoint v2, TexturePoint t0, TexturePoint t1, TexturePoint t2, Texture tex)
    {
      vertices[0] = v0;
      vertices[1] = v1;
      vertices[2] = v2;
      orderVerts();
      findMiddle();

      textured = true;
      texture_points[0] = t0;
      texture_points[1] = t1;
      texture_points[2] = t2;
      texture = tex;

    }

    void drawStrokedTriangle(DrawingWindow& window, double* depthBuffer)
    {
      Line line_1(vertices[0], vertices[1], colour);
      Line line_2(vertices[0], vertices[2], colour);
      Line line_3(vertices[1], vertices[2], colour);
      line_1.bresenham(window, depthBuffer);
      line_2.bresenham(window, depthBuffer);
      line_3.bresenham(window, depthBuffer);
    }

    void drawFilledTriangle(DrawingWindow& window, double* depthBuffer)
    {
      drawStrokedTriangle(window, depthBuffer);
      if(vertices[0].y == vertices[1].y){
        float numberOfSteps = (vertices[2].y - vertices[0].y);
        std::vector<glm::vec3> flat_Bottom = interpolateVec3(glm::vec3(vertices[1].x, vertices[1].y, vertices[1].depth), glm::vec3(vertices[2].x, vertices[2].y, vertices[2].depth), numberOfSteps);
        std::vector<glm::vec3> left_Bottom = interpolateVec3(glm::vec3(vertices[0].x, vertices[0].y, vertices[0].depth), glm::vec3(vertices[2].x, vertices[2].y, vertices[2].depth), numberOfSteps);
        for (int i = 0; i < numberOfSteps; i++) {
            Line line(CanvasPoint(left_Bottom[i].x, left_Bottom[i].y, left_Bottom[i].z), CanvasPoint(flat_Bottom[i].x, flat_Bottom[i].y, flat_Bottom[i].z), colour);
            line.bresenham(window, depthBuffer);
        }
        fillBottom(window, depthBuffer);
      }
      else if(vertices[1].y == vertices[2].y){
        float numberOfSteps = (vertices[2].y - vertices[0].y);
        std::vector<glm::vec3> top_Left = interpolateVec3(glm::vec3(vertices[0].x, vertices[0].y, vertices[0].depth), glm::vec3(vertices[1].x, vertices[1].y, vertices[1].depth), numberOfSteps);
        std::vector<glm::vec3> top_Flat = interpolateVec3(glm::vec3(vertices[0].x, vertices[0].y, vertices[0].depth), glm::vec3(vertices[2].x, vertices[2].y, vertices[2].depth), numberOfSteps);
        for (int i = 0; i < numberOfSteps; i++) {
          Line line(CanvasPoint(top_Left[i].x, top_Left[i].y, top_Left[i].z), CanvasPoint(top_Flat[i].x, top_Flat[i].y, top_Flat[i].z), colour);
          line.bresenham(window, depthBuffer);
        }
        fillTop(window, depthBuffer);
      }
      else{
        CanvasTriangle top_triangle(vertices[0], vertices[1], middlePoint, colour);
        CanvasTriangle bottom_triangle(vertices[1], middlePoint, vertices[2], colour);
        top_triangle.drawFilledTriangle(window, depthBuffer);
        bottom_triangle.drawFilledTriangle(window, depthBuffer);
      }
    }

  private:
    void orderVerts()
    {
      if(vertices[0].y > vertices[1].y) std::swap(vertices[0], vertices[1]);
      if(vertices[0].y > vertices[2].y) std::swap(vertices[0], vertices[2]);
      if(vertices[1].y > vertices[2].y) std::swap(vertices[1], vertices[2]);
    }

    void findMiddle()
    {
      float dY = vertices[2].y - vertices[0].y;
      float dX = vertices[2].x - vertices[0].x;
      double dZ = vertices[2].depth - vertices[0].depth;
      float y = vertices[1].y;
      float x;
      double z;

      if(dX == 0){
        x = vertices[2].x;
      }
      else{
        float m = dY / dX;
        float c = vertices[0].y - (m * vertices[0].x);
        x = (y - c) / m;
      }

      if(dZ == 0){
        z = vertices[2].depth;
      }
      else{
        float m = dY/dZ;
        float c = vertices[0].y - (m  * vertices[0].depth);
        z = (y - c) / m;
      }

      middlePoint = CanvasPoint(x, y, z);

    }

    void fillTop(DrawingWindow& window, double* depthBuffer)
    {
      float dY1 = vertices[1].y - vertices[0].y;
      float dX1 = vertices[1].x - vertices[0].x;
      double dZ1 = vertices[1].depth - vertices[0].depth;
      float dY2 = vertices[2].y - vertices[0].y;
      float dX2 = vertices[2].x - vertices[0].x;
      double dZ2 = vertices[2].depth - vertices[0].depth;

      float x1 = 0;
      float x2 = 0;
      double z1 = 0;
      double z2 = 0;


      for(int y = vertices[0].y; y < vertices[1].y + 1; y++){
        if(dX1 == 0){
          x1 = vertices[0].x;
        }
        else{
          float m1 = dY1 / dX1;
          float c1 = vertices[0].y - ( m1 * vertices[0].x);
          x1 = (y - c1) / m1;
        }

        if(dZ1 == 0){
          z1 = vertices[1].depth;
        }
        else{
          double m = dY1 / dZ1;
          double c = vertices[1].y - ( m * vertices[1].depth );
          z1 = (y - c) / m;
        }

        if(dX2 == 0){
          x2 = vertices[0].x;
        }
        else{
          float m2 = dY2 / dX2;
          float c2 = vertices[0].y - (m2 * vertices[0].x);
          x2 = (y - c2) / m2;
        }

        if(dZ2 == 0){
          z2 = vertices[2].depth;
        }
        else{
          float m = dY2 / dZ2;
          float c = vertices[2].y - ( m * vertices[2].depth );
          z2 = (y - c) / m;
        }

        Line line(CanvasPoint(x1, y, z1), CanvasPoint(x2, y, z2), colour);
        line.drawLine(window, depthBuffer);
      }
    }

    void fillBottom(DrawingWindow& window, double* depthBuffer)
    {
      float dY1 = vertices[0].y - vertices[2].y;
      float dX1 = vertices[0].x - vertices[2].x;
      double dZ1 = vertices[0].depth - vertices[2].depth;
      float dY2 = vertices[1].y - vertices[2].y;
      float dX2 = vertices[1].x - vertices[2].x;
      double dZ2 = vertices[1].depth - vertices[2].depth;

      float x1 = 0;
      float x2 = 0;
      double z1 = 0;
      double z2 = 0;

      for(int y = vertices[0].y; y < vertices[2].y + 1; y++){
        if(dX1 == 0){
          x1 = vertices[0].x;
        }
        else{
          float m = dY1 / dX1;
          float c = vertices[0].y - ( m * vertices[0].x );
          x1 = (y - c) / m;
        }

        if(dZ1 == 0){
          z1 = vertices[0].depth;
        }
        else{
          double m = dY1 / dZ1;
          double c = vertices[0].y - ( m * vertices[0].depth );
          z1 = (y - c) / m;
        }

        if(dX2 == 0){
          x2 = vertices[1].x;
        }
        else{
          float m = dY2 / dX2;
          float c = vertices[1].y - (m * vertices[1].x);
          x2 = (y - c) / m;
        }

        if(dZ2 == 0){
          z2 = vertices[1].depth;
        }
        else{
          float m = dY2 / dZ2;
          float c = vertices[1].y - ( m * vertices[1].depth );
          z2 = (y - c) / m;
        }

        Line line(CanvasPoint(x1, y, z1), CanvasPoint(x2, y, z2), colour);
        line.drawLine(window, depthBuffer);
      }
    }

    std::vector<glm::vec3> interpolateVec3(glm::vec3 from, glm::vec3 to, float numberOfValues)
    {
      std::vector<glm::vec3> output;
      float vecXStep, vecYStep, vecZStep;
      if(numberOfValues == 0){
        output.push_back(glm::vec3(from[0], from[1], from[2]));
        return output;
      }
      if(numberOfValues == 1){
        output.push_back(glm::vec3(from[0], from[1], from[2]));
        output.push_back(glm::vec3(to[0], to[1], to[2]));
        return output;
      }
      else{
        vecXStep = (to[0] - from[0]) / (numberOfValues);
        vecYStep = (to[1] - from[1]) / (numberOfValues);
        vecZStep = (to[2] - from[2]) / (numberOfValues);
        for (int i = 0; i < (numberOfValues); i++) {
          output.push_back(glm::vec3((from[0] + vecXStep*i), (from[1] + vecYStep*i), (from[2] + vecZStep*i)));
        }
      }
      return output;
    }
};

std::ostream& operator<<(std::ostream& os, const CanvasTriangle& triangle)
{
    os << triangle.vertices[0]  << triangle.vertices[1]  << triangle.vertices[2] << triangle.colour<< std::endl;
    return os;
}

#endif
