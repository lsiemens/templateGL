#include "core/camera.h"

#include <glm/gtx/transform.hpp>

Camera::Camera(GLuint shaderID) {
    matrixID = glGetUniformLocation(shaderID, "CameraTransform");
    position = glm::vec3(0, 0, -12);
    direction = glm::vec3(1, 0, 0);
    up = glm::vec3(0, 1, 0);
    FoV = 90.f;
    aspectRatio = 4.f/3.f;
}

glm::mat4 Camera::getViewMatrix() {
    return glm::lookAt(position, position + direction, up);
}

glm::mat4 Camera::getProjectionMatrix() {
    return glm::perspective(glm::radians(FoV), aspectRatio, 0.1f, 100.f);
    //return glm::ortho(-10.f, 10.f, -10.f, 10.f, 0.f, 100.f);
}

void Camera::update() {
    glm::mat4 matrix = getProjectionMatrix()*getViewMatrix();
    glUniformMatrix4fv(matrixID, 1, GL_FALSE, &matrix[0][0]);
}
