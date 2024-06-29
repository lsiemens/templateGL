#ifndef MODEL_H
#define MODEL_H

// keep this before all other OpenGL libraries
#define GLEW_STATIC
#include <GL/glew.h>
#include <glm/glm.hpp>

/// A class for storing model data that can be resued between multiple objects.
class Model {
public:
    GLuint vertexbuffer;
    GLuint colorbuffer;
    unsigned int vertexBufferSize;
    GLuint matrixID;

    Model(GLuint shaderID);
    void releaseBuffers();
    void setVertexBuffer(GLfloat data[], unsigned int bufferSize);
    void setColorBuffer(GLfloat data[], unsigned int bufferSize);
    void drawModel(glm::mat4 modelSpaceToWorldSpace);
};

#endif
