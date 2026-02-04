#ifndef MODEL_H
#define MODEL_H

// keep this before all other OpenGL libraries
#define GLEW_STATIC
#include <GL/glew.h>
#include <glm/glm.hpp>

/// A class for storing model data that can be resued between multiple objects.
class Model {
public:
    GLuint m_vertexbuffer;
    GLuint m_colorbuffer;
    GLsizei m_vertexBufferSize;
    GLuint m_matrixID;

    Model(GLuint shaderID);
    void releaseBuffers();
    void setVertexBuffer(GLfloat data[], GLsizei bufferSize);
    void setColorBuffer(GLfloat data[], GLsizei bufferSize);
    void drawModel(glm::mat4 modelSpaceToWorldSpace);
};

#endif
