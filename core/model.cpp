#include "model.h"

Model::Model(GLuint shaderID) {
    matrixID = glGetUniformLocation(shaderID, "ModelTransform");
}

void Model::releaseBuffers() {
    glDeleteBuffers(1, &vertexbuffer);
    glDeleteBuffers(1, &colorbuffer);
}

void Model::setVertexBuffer(GLfloat data[], unsigned int bufferSize) {
    vertexBufferSize = bufferSize;
    glGenBuffers(1, &vertexbuffer);
    glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
    glBufferData(GL_ARRAY_BUFFER, bufferSize, data, GL_STATIC_DRAW);
}

void Model::setColorBuffer(GLfloat data[], unsigned int bufferSize) {
    glGenBuffers(1, &colorbuffer);
    glBindBuffer(GL_ARRAY_BUFFER, colorbuffer);
    glBufferData(GL_ARRAY_BUFFER, bufferSize, data, GL_STATIC_DRAW);
}

void Model::drawModel(glm::mat4 modelSpaceToWorldSpace) {
    glUniformMatrix4fv(matrixID, 1, GL_FALSE, &modelSpaceToWorldSpace[0][0]);

    glEnableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);

    glEnableVertexAttribArray(1);
    glBindBuffer(GL_ARRAY_BUFFER, colorbuffer);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);

    glDrawArrays(GL_TRIANGLES, 0, vertexBufferSize/(3*sizeof(GLfloat)));

    glDisableVertexAttribArray(1);
    glDisableVertexAttribArray(0);
}
