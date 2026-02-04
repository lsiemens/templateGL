#ifndef SHADERS_H
#define SHADERS_H

#include <string>

// keep this before all other OpenGL libraries
#define GLEW_STATIC
#include <GL/glew.h>

GLuint CompileShader(const std::string& shader_file_path, GLenum shader_type);
GLuint LoadShaders(const std::string& vertex_file_path, const std::string& fragment_file_path);

#endif
