#ifndef BOUNDINGBOX_H
#define BOUNDINGBOX_H

#include "BoundingBox.h"

class BoundingBox
{
    public:

        std::vector<glm::vec3> triVerts;
        std::vector<glm::vec3> boxVerts;
        std::vector<int> containedTriIndexes;
        std::vector<int> containedVertIndexes;
        bool visible = true;


        BoundingBox()
        {
          visible = true;
          for (int i = 0; i < 8; i++) {
            boxVerts.push_back(glm::vec3(0.0f,0.0f,0.0f));
          }
        }

        void updateBoxCoords()
        {
          calcBoxCoords();
        }

        void addTri(ModelTriangle& tri, int triIndex, int v1, int v2, int v3)
        {
          containedTriIndexes.push_back(triIndex);
          for (int i = 0; i < 3; i++) {
            triVerts.push_back(tri.vertices[i]);
          }
          containedVertIndexes.push_back(v1);
          containedVertIndexes.push_back(v2);
          containedVertIndexes.push_back(v3);
          sort(containedVertIndexes.begin(), containedVertIndexes.end());
          containedVertIndexes.erase(unique(containedVertIndexes.begin(), containedVertIndexes.end()), containedVertIndexes.end());
        }

        bool culling(Camera& camera, DrawingWindow& window)
        {
          checkBoxVerts(camera, window);
          return visible;
        }

    private:

        void calcBoxCoords()
        {
          //index 0-3 = 'front face' coords in this order : topLeft, topRight, bottomRight, bottomLeft
          //index 4-7 = 'back face' coords in this order : topLeft, topRight, bottomRight, bottomLeft
          float minX = 0;
          float maxX = 0;
          float minY = 0;
          float maxY = 0;
          float minZ = 0;
          float maxZ = 0;
          for (int i = 0; i < (int) triVerts.size(); i++) {
            if(triVerts[i].x < minX) minX = triVerts[i].x;
            if(triVerts[i].x > maxX) maxX = triVerts[i].x;
            if(triVerts[i].y < minY) minY = triVerts[i].y;
            if(triVerts[i].y > maxY) maxY = triVerts[i].y;
            if(triVerts[i].z < minZ) minZ = triVerts[i].z;
            if(triVerts[i].z > maxZ) maxZ = triVerts[i].z;
          }

          boxVerts[0] = glm::vec3(minX, maxY, minZ); //topLeft Front
          boxVerts[1] = glm::vec3(maxX, maxY, minZ); //topRight Front
          boxVerts[2] = glm::vec3(maxX, minY, minZ); //bottomRight Front
          boxVerts[3] = glm::vec3(minX, minY, minZ); //bottomLeft Front

          boxVerts[4] = glm::vec3(minX, maxY, maxZ); //topLeft back
          boxVerts[5] = glm::vec3(maxX, maxY, maxZ); //topRight back
          boxVerts[6] = glm::vec3(maxX, minY, maxZ); //bottomRight back
          boxVerts[7] = glm::vec3(minX, minY, maxZ); //bottomLeft back
        }

        void checkBoxVerts(Camera& camera, DrawingWindow& window)
        {
          visible = true;
          //bounding box 'view' culling
          int hiddenVerts = 0;
          glm::mat4 worldToCamera =  glm::inverse(camera.cameraToWorld);
          for (size_t j = 0; j < 8; j++) {
            glm::vec4 worldPoint(boxVerts[j].x, boxVerts[j].y, boxVerts[j].z, 1.0f);
            glm::vec4 cameraVert = worldToCamera * worldPoint;
            float f = (camera.focalLength / cameraVert.z);
            float x = -(cameraVert.x * f * window.height/2);
            float y = (cameraVert.y * f * window.height/2);

            if (x < (-window.width/2) || x > (window.width/2)) hiddenVerts ++;
            else if (y < (-window.height/2) || y > (window.height/2)) hiddenVerts ++;
          }
          if(hiddenVerts == 8) visible = false;
        }

};
#endif
