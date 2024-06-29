#ifndef CAMERA_H
#define CAMERA_H

// keep this before all other OpenGL libraries
#define GLEW_STATIC
#include <GL/glew.h>

#include <glm/glm.hpp>

class Camera {
public:
    glm::vec3 position;
    glm::vec3 direction;
    glm::vec3 up;
    float FoV, aspectRatio;
    GLuint matrixID;

    Camera(GLuint shaderID);
    glm::mat4 getViewMatrix();
    glm::mat4 getProjectionMatrix();
    void update();
};

#endif
