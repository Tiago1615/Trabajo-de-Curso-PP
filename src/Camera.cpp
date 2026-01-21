#include "Camera.h"
#include <glm/gtc/matrix_transform.hpp>
#include <cmath>

void Camera::updateFromTrajectory(float x, float y, float z, float theta){
    position = {x, y, z};

    glm::vec3 forward(cos(theta), sin(theta), 0);

    target = position + forward;
}

glm::mat4 Camera::getViewMatrix() const{
    return glm::lookAt(position, target, {0,0,1});
}
