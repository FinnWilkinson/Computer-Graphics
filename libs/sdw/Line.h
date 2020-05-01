#ifndef LINE_H
#define LINE_H

#include "Header.h"

class Line
{
    public:
        CanvasPoint to;
        CanvasPoint from;
        Colour colour;

        Line()
        {
        }

        Line(CanvasPoint to, CanvasPoint from)
        {
            this->to = to;
            this->from = from;
            this->colour = Colour(255, 255, 255);
        }

        Line(CanvasPoint to, CanvasPoint from, Colour colour)
        {
            this->to = to;
            this->from = from;
            this->colour = colour;
        }

        void drawLine(DrawingWindow &window, double* depthBuffer)
        {
            float xDiff = abs(to.x - from.x);
            float yDiff = abs(to.y - from.y);
            float numberOfSteps = std::max(xDiff, yDiff);
            float xStepSize = (to.x - from.x) / numberOfSteps;
            float yStepSize = (to.y - from.y) / numberOfSteps;
            double zStepSize = ((to.depth - from.depth) / numberOfSteps);

            int x, y;
            double z;

            for (float i = 0.0f; i < numberOfSteps; i++){
                x = glm::floor(from.x + (xStepSize * i));
                y = glm::floor(from.y + (yStepSize * i));
                z = from.depth + (zStepSize * i);
                if((x > 0) && (x < window.width) && (y > 0) && (y < window.height)){
                    int index = x + y*window.width;
                    if(z < depthBuffer[index]){
                        depthBuffer[index] = z;
                        window.setPixelColour(x, y, colour.packedColour());
                    }
                }
            }
        }

        void bresenham(DrawingWindow &window, double* depthBuffer)
        {
          bool steep = (std::abs(to.y-from.y) > std::abs(to.x-from.x));
          if(steep){
            std::swap(from.x, from.y);
            std::swap(to.x, to.y);
          }
          if(from.x > to.x){
            std::swap(from, to);
          }

          float dx = to.x-from.x;
          float dy = std::abs(to.y-from.y);
          float dz = to.depth - from.depth;

          int error = (dx * 0.5f);
          int ystep = (from.y < to.y) ? 1 : -1;
          float zStepSize = (dz/dx);
          int y = glm::floor(from.y);
          float z = from.depth;

          int maxX = glm::floor(to.x);

          for (int x = glm::floor(from.x); x <= maxX; x++) {
            if(steep){
              if((x > 0) && (x < window.height) && (y > 0) && (y < window.width)){
                int index = y + x*window.width;
                if(z < depthBuffer[index]){
                    depthBuffer[index] = z;
                    window.setPixelColour(y,x,colour.packedColour());
                }
              }
            }
            else{
              if((x > 0) && (x < window.width) && (y > 0) && (y < window.height)){
                int index = x + y*window.width;
                if(z < depthBuffer[index]){
                    depthBuffer[index] = z;
                    window.setPixelColour(x,y,colour.packedColour());
                }
              }
            }
            z += zStepSize;
            error -= dy;
            if(error < 0){
              y += ystep;
              error += dx;
            }
          }
        }
};

#endif
