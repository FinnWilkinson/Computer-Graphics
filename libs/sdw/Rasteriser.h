#ifndef RASTERISER_H
#define RASTERISER_H

#include "Header.h"

class Rasteriser{
    public:

        Mesh mesh;
        std::vector<CanvasTriangle> wireframe;
        double* depthBuffer;
        bool rasteriseOn;
        bool wireframeOn;

        Rasteriser()
        {

        }

        Rasteriser(Mesh mesh, DrawingWindow window)
        {
            this->mesh = mesh;
            this->depthBuffer = (double*)std::malloc(window.height * window.width* sizeof(double));
            this->rasteriseOn = false;
            this->wireframeOn = false;
        }

        void draw(Camera& camera, DrawingWindow& window){
            mesh.cullTris(camera, window);
            genWireframe(camera, window);
            window.clearPixels();
            int len = wireframe.size();

            for(int i=0; i<len; i++)
            {
                if(rasteriseOn) wireframe[i].drawFilledTriangle(window, depthBuffer);
                else if (wireframeOn) wireframe[i].drawStrokedTriangle(window, depthBuffer);
            }
        }

        void animateModel(const int& frame_count, const int sizeOfGrid)
        {
          mesh.model = mesh.originalModel;
          const int startIndex = mesh.boundingBoxes[(int)mesh.boundingBoxes.size()-1].containedVertIndexes[0];
          int vertexLines[33];
          int addSubtractScalars[2*33];
          for (int i = 0; i < sizeOfGrid; i++) {
            vertexLines[i] = i;
          }
          int divisionsSize = 2*(sizeOfGrid)/6;
          for (int i = 0; i < divisionsSize; i++) {
            addSubtractScalars[i] = 1;
            addSubtractScalars[i+divisionsSize] = -1;
            addSubtractScalars[i+(divisionsSize*2)] = 1;
            addSubtractScalars[i+(divisionsSize*3)] = -1;
            addSubtractScalars[i+(divisionsSize*4)] = 1;
            addSubtractScalars[i+(divisionsSize*5)] = -1;
          }
          float currentVal = sin(M_PI/1.5)/divisionsSize;
          if(frame_count < sizeOfGrid){
            for (int i = 0; i <= (frame_count%sizeOfGrid); i++) {
              for (int j = 0; j < sizeOfGrid; j++) {
                int vertexIndex = vertexLines[j] + sizeOfGrid*i + startIndex;
                int scalarIndex = (frame_count-i)%(2*sizeOfGrid);
                float val = currentVal*addSubtractScalars[scalarIndex];
                for (int i = 0; i < (int) mesh.verts[vertexIndex].triangles.size(); i++) {
                  int modelIndex = mesh.verts[vertexIndex].triangles[i];
                  if(mesh.model[modelIndex].vertices[0] == mesh.verts[vertexIndex].position) mesh.model[modelIndex].vertices[0].y += val;
                  else if(mesh.model[modelIndex].vertices[1] == mesh.verts[vertexIndex].position) mesh.model[modelIndex].vertices[1].y += val;
                  else if(mesh.model[modelIndex].vertices[2] == mesh.verts[vertexIndex].position) mesh.model[modelIndex].vertices[2].y += val;
                }
                mesh.verts[vertexIndex].position.y += val;
              }
            }
          }
          else {
            for (int i = 0; i < sizeOfGrid; i++) {
              for (int j = 0; j < sizeOfGrid; j++) {
                int vertexIndex = vertexLines[j] + sizeOfGrid*i + startIndex;
                int scalarIndex = (frame_count-i)%(2*sizeOfGrid);
                float val = currentVal*addSubtractScalars[scalarIndex];
                for (int i = 0; i < (int) mesh.verts[vertexIndex].triangles.size(); i++) {
                  int modelIndex = mesh.verts[vertexIndex].triangles[i];
                  if(mesh.model[modelIndex].vertices[0] == mesh.verts[vertexIndex].position) mesh.model[modelIndex].vertices[0].y += val;
                  else if(mesh.model[modelIndex].vertices[1] == mesh.verts[vertexIndex].position) mesh.model[modelIndex].vertices[1].y += val;
                  else if(mesh.model[modelIndex].vertices[2] == mesh.verts[vertexIndex].position) mesh.model[modelIndex].vertices[2].y += val;
                }
                mesh.verts[vertexIndex].position.y += val;
              }
            }
          }
          mesh.originalModel = mesh.model;
        }


    private:

        void genWireframe(Camera& camera, DrawingWindow& window)
        {
            int len = mesh.model.size();
            float x, y;
            double z;
            CanvasPoint verts[3];
            glm::mat4 worldToCamera =  glm::inverse(camera.cameraToWorld);
            resetDepthBuffer(window);

            wireframe.clear();

            for(int i=0; i<len; i++){
              if(mesh.model[i].visible == false) continue;
                for(int j=0; j<3; j++){
                    glm::vec4 worldPoint(mesh.model[i].vertices[j].x, mesh.model[i].vertices[j].y, mesh.model[i].vertices[j].z, 1);
                    glm::vec4 cameraVert = worldToCamera * worldPoint;
                    float f = (camera.focalLength / cameraVert.z);
                    x = -(cameraVert.x * f * window.height/2) + window.width/2;
                    y = (cameraVert.y * f * window.height/2) + window.height/2;
                    z = 1.0f / cameraVert.z;
                    CanvasPoint point(glm::floor(x), glm::floor(y), z);
                    verts[j] = point;
                }
                if(mesh.model[i].textured){
                    TexturePoint ts[3] = mesh.model[i].texture_points;
                    CanvasTriangle tri(verts[0], verts[1], verts[2], ts[0], ts[1], ts[2], mesh.texture);
                    wireframe.push_back(tri);
                }
                else{
                    CanvasTriangle tri(verts[0], verts[1], verts[2], mesh.model[i].colour);
                    wireframe.push_back(tri);
                }
            }
        }

        void resetDepthBuffer(DrawingWindow& window)
        {
            for(int y = 0; y < window.height; y++){
                for(int x = 0; x < window.width; x++){
                    depthBuffer[x + y*window.width] = 0;
                }
            }
        }
};

#endif
