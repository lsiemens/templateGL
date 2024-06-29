#ifndef SHADERS_H
#define SHADERS_H

// keep this before all other OpenGL libraries
#define GLEW_STATIC
#include <GL/glew.h>

GLuint CompileShader(const char* shader_file_path, GLenum shader_type);
GLuint LoadShaders(const char* vertex_file_path, const char* fragment_file_path);

#endif
