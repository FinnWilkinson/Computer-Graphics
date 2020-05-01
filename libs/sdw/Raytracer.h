#ifndef RAYTRACER_H
#define RAYTRACER_H

#include "Header.h"

class Raytracer{
    public:

        Mesh wireframe;
        bool raytracerOn;
        Light light;

        Raytracer(){

        }

        Raytracer(Mesh wireframe, glm::vec3 position, float intensity) {
            this->wireframe = wireframe;
            this->raytracerOn = false;
            this->light = Light(position, intensity);
        }

        void render(DrawingWindow& window, Camera& camera)
        {
            wireframe.cullTris(camera, window);
            raytracing(window, camera);
            std::cout << "RAY TRACE COMPLETE" << std::endl;
        }

        void animateLight(int frame_count)
        {
          if(frame_count < 50){
            //light down
            light.position = light.position + glm::vec3(0.0f, -0.05f, 0.0f);
          }
          else if(frame_count < 100){
            //light up
            light.position = light.position + glm::vec3(0.0f, 0.05f, 0.0f);
          }
          else if(frame_count < 150){
            //light forward
            light.position = light.position + glm::vec3(0.0f, 0.0f, 0.15f);
          }
          else if(frame_count < 200){
            //light backward
            light.position = light.position + glm::vec3(0.0f, 0.0f, -0.15f);
          }
          else if(frame_count < 250){
            //light down
            light.position = light.position + glm::vec3(0.0f, -0.05f, 0.0f);
          }
          else if(frame_count < 300){
            //light up
            light.position = light.position + glm::vec3(0.0f, 0.05f, 0.0f);
          }
        }

    private:

        void raytracing(DrawingWindow& window, Camera& camera)
        {
          for(int x = 0; x < window.width; x++){
            for(int y = 0; y < window.height; y++){
              Ray ray(x, y);
              ray.raytrace(wireframe.model, camera, window, light, wireframe, true);
            }
          }
        }
};

#endif
