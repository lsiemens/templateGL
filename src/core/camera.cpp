#include "core/camera.h"

#include <glm/gtx/transform.hpp>

Camera::Camera(GLuint shaderID) {
    m_matrixID = glGetUniformLocation(shaderID, "CameraTransform");
    m_position = glm::vec3(0, 0, -12);
    m_direction = glm::vec3(1, 0, 0);
    m_up = glm::vec3(0, 1, 0);
    m_FoV = 90.f;
    m_aspectRatio = 4.f/3.f;
}

glm::mat4 Camera::getViewMatrix() {
    return glm::lookAt(m_position, m_position + m_direction, m_up);
}

glm::mat4 Camera::getProjectionMatrix() {
    return glm::perspective(glm::radians(m_FoV), m_aspectRatio, 0.1f, 100.f);
    //return glm::ortho(-10.f, 10.f, -10.f, 10.f, 0.f, 100.f);
}

void Camera::update() {
    glm::mat4 matrix = getProjectionMatrix()*getViewMatrix();
    glUniformMatrix4fv(m_matrixID, 1, GL_FALSE, &matrix[0][0]);
}
