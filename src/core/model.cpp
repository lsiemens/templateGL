#include "core/model.h"

Model::Model(GLuint shaderID) {
    m_matrixID = glGetUniformLocation(shaderID, "ModelTransform");
}

void Model::releaseBuffers() {
    glDeleteBuffers(1, &m_vertexbuffer);
    glDeleteBuffers(1, &m_colorbuffer);
}

void Model::setVertexBuffer(GLfloat data[], GLsizei bufferSize) {
    m_vertexBufferSize = bufferSize;
    glGenBuffers(1, &m_vertexbuffer);
    glBindBuffer(GL_ARRAY_BUFFER, m_vertexbuffer);
    glBufferData(GL_ARRAY_BUFFER, bufferSize, data, GL_STATIC_DRAW);
}

void Model::setColorBuffer(GLfloat data[], GLsizei bufferSize) {
    glGenBuffers(1, &m_colorbuffer);
    glBindBuffer(GL_ARRAY_BUFFER, m_colorbuffer);
    glBufferData(GL_ARRAY_BUFFER, bufferSize, data, GL_STATIC_DRAW);
}

void Model::drawModel(glm::mat4 modelSpaceToWorldSpace) {
    glUniformMatrix4fv(m_matrixID, 1, GL_FALSE, &modelSpaceToWorldSpace[0][0]);

    glEnableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, m_vertexbuffer);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);

    glEnableVertexAttribArray(1);
    glBindBuffer(GL_ARRAY_BUFFER, m_colorbuffer);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);

    glDrawArrays(GL_TRIANGLES, 0, m_vertexBufferSize/(3*sizeof(GLfloat)));

    glDisableVertexAttribArray(1);
    glDisableVertexAttribArray(0);
}
