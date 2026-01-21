#pragma once
#include <glm/glm.hpp>

class Camera
{
    public:
        void updateFromTrajectory(float x, float y, float z, float theta);

        glm::mat4 getViewMatrix() const;
        glm::vec3 getPosition() const { return position; }
    private:
        glm::vec3 position {0,0,0};
        glm::vec3 target {0,1,0};
};
