#ifndef CAMERA_H
#define CAMERA_H

// keep this before all other OpenGL libraries
#define GLEW_STATIC
#include <GL/glew.h>

#include <glm/glm.hpp>

class Camera {
public:
    glm::vec3 m_position;
    glm::vec3 m_direction;
    glm::vec3 m_up;
    float m_FoV, m_aspectRatio;
    GLuint m_matrixID;

    Camera(GLuint shaderID);
    glm::mat4 getViewMatrix();
    glm::mat4 getProjectionMatrix();
    void update();
};

#endif
