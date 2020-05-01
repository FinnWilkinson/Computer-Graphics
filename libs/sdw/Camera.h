#include "Header.h"

#ifndef CAMERA_H
#define CAMERA_H



class Camera
{
    public:

        glm::mat4 cameraToWorld;
        bool orbitMode;
        float focalLength = 5.8f;
        float nearPlaneDist = 0.1f;
        float farPlaneDist = 50.0f;

        Camera()
        {

        }

        Camera(float x, float y, float z)
        {
            this->cameraToWorld = glm::mat4(glm::vec4(1.0f, 0.0f,  0.0f, 0.0f),
                                            glm::vec4(0.0f, 1.0f,  0.0f, 0.0f),
                                            glm::vec4(0.0f, 0.0f,  1.0f, 0.0f),
                                            glm::vec4(   x,    y,     z, 1.0f));
            this->orbitMode = false;
        }

        void performRotation(const float& angle, const char& axis)
        {
            glm::vec4 cameraPos(cameraToWorld[3][0], cameraToWorld[3][1], cameraToWorld[3][2], cameraToWorld[3][3]);
            glm::mat4 rotationMatrix;

            if(axis == 'x') xRotationMatrix(angle, rotationMatrix);
            else yRotationMatrix(angle, rotationMatrix);
            cameraToWorld = rotationMatrix * cameraToWorld;
            if(!orbitMode) cameraToWorld[3] = cameraPos;
        }

        void performTranslation(const float& value, const char& axis)
        {
            if(axis == 'x') cameraToWorld[3][0] += value;
            else if(axis == 'y') cameraToWorld[3][1] += value;
            else cameraToWorld[3][2] += value;
        }

        void orbit()
        {
            orbitMode = !orbitMode;
            if(orbitMode) std::cout << "ORBIT MODE ON" << std::endl;
            else std::cout << "ORBIT MODE OFF" << std::endl;
        }

        void getCameraPosition(glm::vec3& cameraPosition)
        {
            cameraPosition.x = cameraToWorld[3][0];
            cameraPosition.y = cameraToWorld[3][1];
            cameraPosition.z = cameraToWorld[3][2];
        }

    private:

        float radians(float angle)
        {
            return angle * (M_PI/180);
        }

        void xRotationMatrix(const float& angle, glm::mat4& matrix)
        {
            matrix = glm::mat4(glm::vec4(1.0f, 0.0f                     ,  0.0f                    , 0.0f),
                                glm::vec4(0.0f, glm::cos(radians(angle)), -glm::sin(radians(angle)), 0.0f),
                                glm::vec4(0.0f, glm::sin(radians(angle)),  glm::cos(radians(angle)), 0.0f),
                                glm::vec4(0.0f, 0.0f                    ,  0.0f                    , 1.0f));
        }

        void yRotationMatrix(const float& angle, glm::mat4& matrix)
        {
            matrix = glm::mat4(glm::vec4( glm::cos(radians(angle)), 0.0f, glm::sin(radians(angle)), 0.0f),
                               glm::vec4( 0.0f                    , 1.0f, 0.0f                    , 0.0f),
                               glm::vec4(-glm::sin(radians(angle)), 0.0f, glm::cos(radians(angle)), 0.0f),
                               glm::vec4( 0.0f                    , 0.0f, 0.0f                    , 1.0f));
        }


};

#endif
