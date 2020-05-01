#ifndef LIGHT_H
#define LIGHT_H

#include "Header.h"

class Light {
    public:

        glm::vec3 position;
        double intensity;

        Light() {

        }

        Light(glm::vec3 position, float intensity){
            this->position = position;
            this->intensity = intensity;
        }

        void calculatePointBrightness(RayTriangleIntersection& intersection)
        {
            double distance = glm::distance(position, intersection.worldPoint);
            double brightness;
            if(!intersection.inShadow) brightness = std::min(1.0, std::max(0.2, ((intensity * intersection.incidence) / (4.0f * M_PI * glm::pow(distance, 2)))));
            else brightness = 0.2f;
            intersection.intersectedTriangle.colour.brightness = brightness;
        }

        void calculateIncidenceLighting(RayTriangleIntersection& intersection)
        {   
            glm::vec3 light_vect = glm::normalize(position - intersection.worldPoint);
            float angle = glm::dot(intersection.intersectedTriangle.normal, light_vect);
            intersection.incidence = angle;
        }

        void gouraurdShading(RayTriangleIntersection& intersection)
        {
            const glm::vec3 light_vect = glm::normalize(position - intersection.worldPoint);
            float* brightness = (float*)malloc(sizeof(float) * 3);
            for(int i = 0; i <3; i++){
                brightness[i] = glm::dot(intersection.intersectedTriangle.vertex_normals[i], light_vect);
            }
            float angle = (1 - intersection.uv.x - intersection.uv.y) * brightness[0] + intersection.uv.x * brightness[1] + intersection.uv.y * brightness[2];
            intersection.incidence = angle;
        }

        void phongShading(RayTriangleIntersection& intersection)
        {
            const glm::vec3 light_vect = glm::normalize(position - intersection.worldPoint);
            const glm::vec3 vn0 = intersection.intersectedTriangle.vertex_normals[0];
            const glm::vec3 vn1 = intersection.intersectedTriangle.vertex_normals[1];
            const glm::vec3 vn2 = intersection.intersectedTriangle.vertex_normals[2];
            glm::vec3 normal = (1 - intersection.uv.x - intersection.uv.y) * vn0 + intersection.uv.x * vn1 + intersection.uv.y * vn2;
            normal = glm::normalize(normal);
            float angle = glm::dot(normal, light_vect);
            intersection.incidence = angle;
        }

        void calculateShadow(std::vector<ModelTriangle>& triangles, RayTriangleIntersection& intersection)
        {   
            int len = triangles.size();
            glm::vec3 shadowPoint = intersection.worldPoint + (0.1f * intersection.intersectedTriangle.normal); 
            double distToLight = glm::distance(position, shadowPoint);
            if(intersection.intersectedTriangle.colour.specular) return;
            for(int i = 0; i < len; i++)
            {
                if(i != intersection.intersectedIndex){
                    glm::vec3 e0 = triangles[i].vertices[1] - triangles[i].vertices[0];
                    glm::vec3 e1 = triangles[i].vertices[2] - triangles[i].vertices[0];
                    glm::vec3 SPVector = shadowPoint - triangles[i].vertices[0];
                    glm::vec3 rayDirection = glm::normalize(position - shadowPoint);
                    glm::mat3 DEMatrix(-rayDirection, e0, e1);
                    glm::vec3 possibleIntersection = glm::inverse(DEMatrix) * SPVector;

                    float t = possibleIntersection[0];
                    float u = possibleIntersection[1];
                    float v = possibleIntersection[2];

                    if(constraints(u, v)){
                        if(t > 0 && t < distToLight){
                            intersection.inShadow = true;
                        }
                    }
                }
            }
        }

    private:
        bool constraints(const float& u, const float& v)
        {
            if(0.0f <= u && u <= 1.0f){
                if(0.0f <= v && v <= 1.0f){
                    if(u + v <= 1.0f){
                        return true;
                    }
                }
            }
            return false;
        }

        void interpolateBrightness(ModelTriangle& triangle)
        {
            
        }
};    


#endif