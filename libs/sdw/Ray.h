#ifndef RAY_H
#define RAY_H

#include "Header.h"

class Ray{
    public:

        CanvasPoint screenPoint;
        glm::vec4 worldPoint;
        glm::vec3 rayDirection;
        RayTriangleIntersection intersection;
        bool intersected;
        const int max_reflections = 1;
        int reflections;
        bool reflected = false;
        bool hit_glass = false;
        Colour glass_colour;
        Colour reflected_colour;
        Colour final_colour;

        Ray()
        {

        }

        Ray(const float& x, const float& y){
            this->screenPoint = CanvasPoint(x, y);
            this->intersection = RayTriangleIntersection();
            this->intersected = false;
            reflections = 0;
            glass_colour = Colour(0, 0, 0, 0.0f);
            reflected_colour = Colour(0, 0, 0, 0.0f);
            final_colour = Colour(0, 0, 0, 0.0f);
        }

        void raytrace(std::vector<ModelTriangle>& triangles, Camera& camera, DrawingWindow& window, Light& light, Mesh& wireframe, const bool& ssaa)
        {
          pixelToWorld(window, camera);
          glm::vec3 cameraPosition(camera.cameraToWorld[3][0], camera.cameraToWorld[3][1], camera.cameraToWorld[3][2]);
          for(int i = 0; i < (int) triangles.size(); i++)
          {
            closestIntersection(triangles[i], i, cameraPosition, window, wireframe, camera);
          }
          if((intersected) && (ssaa)){
              SSAA(intersection.screenPoint, intersection.ssaaColour, window, camera, light, wireframe);
              window.setPixelColour(intersection.screenPoint.x, intersection.screenPoint.y, intersection.ssaaColour.packedColour());
          }
        }

    private:
        void pixelToWorld(DrawingWindow& window, Camera& camera)
        {
            worldPoint = glm::vec4(0.0f, 0.0f, -camera.focalLength, 1.0f);
            worldPoint.x = ((screenPoint.x  - window.width/2) / (window.height/2));
            worldPoint.y = -((screenPoint.y - window.height/2) / (window.height/2));
            worldPoint = camera.cameraToWorld * worldPoint;
            reduceVect(glm::normalize(worldPoint - camera.cameraToWorld[3]));
        }

        void reduceVect(glm::vec4 rayDirection)
        {
            this->rayDirection.x = rayDirection[0];
            this->rayDirection.y = rayDirection[1];
            this->rayDirection.z = rayDirection[2];
        }

        void closestIntersection(ModelTriangle& triangle, const int& i, glm::vec3& source, DrawingWindow& window, Mesh& wireframe, Camera& camera)
        {
            if(triangle.visible == false) return;

            glm::vec3 e0 = triangle.vertices[1] - triangle.vertices[0];
            glm::vec3 e1 = triangle.vertices[2] - triangle.vertices[0];
            glm::vec3 SPVector = source - triangle.vertices[0];
            glm::mat3 DEMatrix(-rayDirection, e0, e1);
            glm::vec3 possibleSolution = glm::inverse(DEMatrix) * SPVector;

            float t = possibleSolution.x;
            float u = possibleSolution.y;
            float v = possibleSolution.z;

            if(constraints(u, v)){
                if(t > 0 && t < intersection.distanceFromCamera){
                    intersected = true;
                    intersection.worldPoint = (source + (t*rayDirection));
                    intersection.distanceFromCamera = t;
                    intersection.uv = glm::vec2(u, v);
                    intersection.intersectedTriangle = triangle;
                    intersection.intersectedIndex = i;
                    intersection.screenPoint = CanvasPoint(screenPoint.x, screenPoint.y);
                    if(triangle.textured) getTexturePointColour(triangle, intersection.uv, wireframe);
                }
            }
        }

        bool constraints(const float& u, const float& v)
        {
          if(u < 0.0f || u > 1.0f) return false;
          else if(v < 0.0f || v > 1.0f) return false;
          else if(u + v <= 1.0f) return true;
          else return false;
        }

        void SSAA(CanvasPoint& screenPoint, Colour& colour, DrawingWindow& window, Camera& camera, Light& light, Mesh& wireframe)
        {

            const int samples = 8;
            float x = screenPoint.x;
            float y = screenPoint.y;
            float offsets_x[samples] = {0.4375, -0.1875, 0.0625, -0.4375, 0.3125, -0.0625, -0.3125, 0.1875};
            float offsets_y[samples] = {-0.4375, -0.3125, -0.1875, -0.0625, 0.0625, 0.1875, 0.3125, 0.4375};

            Colour average_colour(0, 0, 0, 0.0f);

            for(int i = 0; i < samples; i++){
                Ray sampler(x+offsets_x[i], y+offsets_y[i]);
                sampler.raytrace(wireframe.model, camera, window, light, wireframe, false);
                if(sampler.intersected){
                  if(sampler.intersection.intersectedTriangle.colour.specular){
                    handleReflectionAndRefraction(sampler, light, wireframe);
                    combineColour(final_colour, sampler.final_colour, 1.0f, 1.0f);
                  }
                  else{
                    lighting(light, sampler.intersection, wireframe);
                    combineColour(final_colour, sampler.intersection.intersectedTriangle.colour, 1.0f, 1.0f);
                  }
                }
            }
            final_colour.red /= samples;
            final_colour.green /= samples;
            final_colour.blue /= samples;
            final_colour.brightness /= samples;
            colour = final_colour;
        }

        void handleReflectionAndRefraction(Ray& sampler, Light& light, Mesh& wireframe)
        {
          bool hasReflection = (sampler.intersection.intersectedTriangle.colour.reflective_index > 0.0f);
          bool hasTransparency = (sampler.intersection.intersectedTriangle.colour.transparency > 0.0f);
          
          if(hasReflection){
            handleReflection(sampler, light, wireframe);
          }
          if(hasTransparency){
            handleTransparency(sampler, light, wireframe);
          }

          if(sampler.hit_glass && !sampler.reflected){
            combineColour(sampler.final_colour, sampler.glass_colour, (1.0f - sampler.glass_colour.transparency) , 0.0f);
            combineColour(sampler.final_colour, sampler.intersection.intersectedTriangle.colour, sampler.glass_colour.transparency, 1.0f);
          }
          else if(!sampler.hit_glass && sampler.reflected){
            combineColour(sampler.final_colour, sampler.reflected_colour, 1.0f - sampler.reflected_colour.reflective_index, 1.0f - sampler.reflected_colour.reflective_index);
            combineColour(sampler.final_colour, sampler.intersection.intersectedTriangle.colour, sampler.reflected_colour.reflective_index, sampler.reflected_colour.reflective_index);
          }
          else{
            combineColour(sampler.final_colour, sampler.glass_colour, (1.0f - sampler.glass_colour.transparency), 0.0f);
            combineColour(sampler.final_colour, sampler.reflected_colour, (1.0f - sampler.reflected_colour.reflective_index) * sampler.glass_colour.transparency, (1.0f - sampler.reflected_colour.reflective_index));
            combineColour(sampler.final_colour, sampler.intersection.intersectedTriangle.colour, sampler.glass_colour.transparency, sampler.reflected_colour.reflective_index);
          }

        }

        void combineColour(Colour& first, const Colour& second, const float& value, const float& shading_value)
        {
          first.red += second.red * value;
          first.green += second.green * value;
          first.blue += second.blue * value;
          first.brightness += second.brightness * shading_value;
        }

        void handleTransparency(Ray& sampler, Light& light, Mesh& wireframe)
        {
          if(!sampler.hit_glass){
            sampler.hit_glass = true;
            sampler.glass_colour = sampler.intersection.intersectedTriangle.colour;
          }
          transparency(wireframe.model, sampler);
          findRefractedRayIntersection(wireframe.model, sampler, wireframe);
          if(sampler.intersected && sampler.intersection.intersectedTriangle.colour.reflective_index > 0.0f){
              handleReflection(sampler, light, wireframe);
          }
          else{
            if(!sampler.intersected) sampler.intersection.intersectedTriangle.colour = Colour(0, 0, 0, 0.0f);
            else lighting(light, sampler.intersection, wireframe);
          }
        }


        void handleReflection(Ray& sampler, Light& light, Mesh& wireframe)
        { 
          if(!sampler.reflected){
            sampler.reflected = true;
            lighting(light, sampler.intersection, wireframe);
            sampler.reflected_colour = sampler.intersection.intersectedTriangle.colour;
          }
          reflection(wireframe.model, sampler, wireframe);
          sampler.reflections++;
          if(sampler.intersected){
            if(sampler.intersection.intersectedTriangle.colour.reflective_index > 0.0f && sampler.reflections < sampler.max_reflections){
              handleReflection(sampler, light, wireframe);
            }
            else if(sampler.intersection.intersectedTriangle.colour.transparency > 0.0f){
              handleTransparency(sampler, light, wireframe);
            }
            else{
              lighting(light, sampler.intersection, wireframe);
            }
          }
          else sampler.intersection.intersectedTriangle.colour = Colour(0, 0, 0, 0.0f);
        }

        void interpolateNormal(Ray& sampler)
        {
          const glm::vec3 vn0 = sampler.intersection.intersectedTriangle.vertex_normals[0];
          const glm::vec3 vn1 = sampler.intersection.intersectedTriangle.vertex_normals[1];
          const glm::vec3 vn2 = sampler.intersection.intersectedTriangle.vertex_normals[2];
          sampler.intersection.intersectedTriangle.normal = (1 - sampler.intersection.uv.x - sampler.intersection.uv.y) * vn0 + sampler.intersection.uv.x * vn1 + sampler.intersection.uv.y * vn2;
          sampler.intersection.intersectedTriangle.normal = glm::normalize(sampler.intersection.intersectedTriangle.normal);
        }
 
        void reflection(std::vector<ModelTriangle>& triangles, Ray& sampler, Mesh& wireframe)
        {
          int len = triangles.size();
          float depth = INFINITY;
          float reflectionBias = 0.0001;
          interpolateNormal(sampler);
          glm::vec3 reflection_normal = sampler.intersection.intersectedTriangle.normal;
          glm::vec3 reflection_point = sampler.intersection.worldPoint;
          glm::vec3 reflected_ray = glm::reflect(sampler.rayDirection, reflection_normal);
          sampler.rayDirection = reflected_ray;
          sampler.intersected = false;

          for(int i  = 0; i < len; i++){
            glm::vec3 e0 = triangles[i].vertices[1] - triangles[i].vertices[0];
            glm::vec3 e1 = triangles[i].vertices[2] - triangles[i].vertices[0];
            glm::vec3 SPVector = reflection_point - triangles[i].vertices[0];
            glm::mat3 DEMatrix(-reflected_ray, e0, e1);
            glm::vec3 possibleSolution = glm::inverse(DEMatrix) * SPVector;

            float t = possibleSolution.x;
            float u = possibleSolution.y;
            float v = possibleSolution.z;

            if(constraints(u, v)){
                if(t < depth && t > reflectionBias){
                  depth = t;
                  sampler.intersected = true;
                  sampler.intersection.uv = glm::vec2(u, v);
                  sampler.intersection.worldPoint = (reflection_point + (t*reflected_ray));
                  sampler.intersection.distanceFromCamera = t;
                  sampler.intersection.intersectedTriangle = triangles[i];
                  sampler.intersection.intersectedIndex = i;
                  if(triangles[i].textured) getTexturePointColour(triangles[i], sampler.intersection.uv, wireframe);
                }
            }
          }
        }

        void transparency(std::vector<ModelTriangle>& triangles, Ray& sampler)
        {
          int len = triangles.size();
          float depth = INFINITY;
          interpolateNormal(sampler);
          const RayTriangleIntersection first_intersection = sampler.intersection;
          glm::vec3 surface_normal = sampler.intersection.intersectedTriangle.normal;
          glm::vec3 hit_point = first_intersection.worldPoint;
          refract(sampler.rayDirection, surface_normal, sampler.intersection.intersectedTriangle.colour.refractive_index);
          glm::vec3 refracted_ray = sampler.rayDirection;
          sampler.intersected = false;
          for(int i = 0; i < len; i++){
            if(glm::dot(refracted_ray, triangles[i].normal) < 0 || i == first_intersection.intersectedIndex) continue;
            glm::vec3 e0 = triangles[i].vertices[1] - triangles[i].vertices[0];
            glm::vec3 e1 = triangles[i].vertices[2] - triangles[i].vertices[0];
            glm::vec3 SPVector = hit_point - triangles[i].vertices[0];
            glm::mat3 DEMatrix(-refracted_ray, e0, e1);
            glm::vec3 possibleSolution = glm::inverse(DEMatrix) * SPVector;

            float t = possibleSolution.x;
            float u = possibleSolution.y;
            float v = possibleSolution.z;
            if(constraints(u, v)){
                if(t < depth && t > 0){
                  depth = t;
                  sampler.intersected = true;
                  sampler.intersection.uv = glm::vec2(u, v);
                  sampler.intersection.worldPoint = (hit_point + (t*refracted_ray));
                  sampler.intersection.intersectedTriangle = triangles[i];
                  sampler.intersection.intersectedIndex = i;
                }
            }

          }

        }

        void findRefractedRayIntersection(std::vector<ModelTriangle>& triangles, Ray& sampler, Mesh& wireframe)
        {
          int len = triangles.size();
          float depth = INFINITY;
          const RayTriangleIntersection refracted_intersection = sampler.intersection;
          interpolateNormal(sampler);
          refract(sampler.rayDirection, sampler.intersection.intersectedTriangle.normal, sampler.intersection.intersectedTriangle.colour.refractive_index);
          glm::vec3 rayDirection = sampler.rayDirection;
          glm::vec3 hit_point = sampler.intersection.worldPoint;
          sampler.intersected = false;
          for(int i = 0; i < len; i++){
            if(i == refracted_intersection.intersectedIndex) continue;
            glm::vec3 e0 = triangles[i].vertices[1] - triangles[i].vertices[0];
            glm::vec3 e1 = triangles[i].vertices[2] - triangles[i].vertices[0];
            glm::vec3 SPVector = hit_point - triangles[i].vertices[0];
            glm::mat3 DEMatrix(-rayDirection, e0, e1);
            glm::vec3 possibleSolution = glm::inverse(DEMatrix) * SPVector;

            float t = possibleSolution.x;
            float u = possibleSolution.y;
            float v = possibleSolution.z;
            if(constraints(u, v)){
                if(t < depth && t > 0){
                  depth = t;
                  sampler.intersected = true;
                  sampler.intersection.uv = glm::vec2(u, v);
                  sampler.intersection.worldPoint = (hit_point + (t*rayDirection));
                  sampler.intersection.intersectedTriangle = triangles[i];
                  sampler.intersection.intersectedIndex = i;
                  if(triangles[i].textured) getTexturePointColour(triangles[i], sampler.intersection.uv, wireframe);
                }
            }
          }

        }

        void refract(glm::vec3& i, glm::vec3& n, float ref_index){
          ref_index = 2.0f - ref_index;
          float cosi = glm::dot(i, n);
          i = glm::normalize((i * ref_index - n * (-cosi + ref_index * cosi)));
        }

        void lighting(Light& light, RayTriangleIntersection& intersection, Mesh& wireframe)
        {
            //light.calculateIncidenceLighting(intersection);
            //light.gouraurdShading(intersection);
            light.phongShading(intersection);
            light.calculateShadow(wireframe.model, intersection);
            light.calculatePointBrightness(intersection);
        }

        void getTexturePointColour(ModelTriangle& triangle, const glm::vec2& uv, const Mesh& wireframe)
        {
          glm::vec2 t0 = triangle.texture_points[0].point;
          glm::vec2 t1 = triangle.texture_points[1].point;
          glm::vec2 t2 = triangle.texture_points[2].point;
          glm::vec2 tex_point = t0 + (glm::vec2(uv.x, uv.x) * (t1 - t0)) + (glm::vec2(uv.y, uv.y) * (t2 - t0));
          int x = tex_point.x * wireframe.texture.image_width;
          int y = tex_point.y * wireframe.texture.image_height;
          triangle.colour.red = wireframe.texture.colours[x + y*wireframe.texture.image_width].red;
          triangle.colour.green = wireframe.texture.colours[x + y*wireframe.texture.image_width].green;
          triangle.colour.blue = wireframe.texture.colours[x + y*wireframe.texture.image_width].blue;
        }
};

#endif
