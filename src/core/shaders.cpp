#include "core/shaders.h"

#include <vector>
#include <string>
#include <iostream>
#include <fstream>
#include <sstream>
#include <stdexcept>

#include "path_util.h"

GLuint CompileShader(const std::string& shader_file_path, GLenum shader_type){
    GLuint ShaderID = glCreateShader(shader_type);

    std::string fixed_path;
    try {
        fixed_path = get_fixed_path(shader_file_path).string();
    } catch (...) {
        std::cerr << "Failed to get absolute path of '" << shader_file_path << "'.\n";
        throw;
    }

    //Get Code
    std::string ShaderCode;
    std::ifstream ShaderStream(fixed_path, std::ios::in);
    if (ShaderStream.is_open()) {
        std::stringstream shader_code;
        shader_code << ShaderStream.rdbuf();
        ShaderCode = shader_code.str();
        ShaderStream.close();
    } else {
        std::cerr << "Failed to open '" << shader_file_path << "'.\n";
        throw std::runtime_error("Could not load shader source code.");
    }

    GLint Result = GL_FALSE;
    int InfoLogLength;

    //Compile Shader
    std::clog << "Compiling shader : " << shader_file_path << "\n";
    char const* SourcePointer = ShaderCode.c_str();
    glShaderSource(ShaderID, 1, &SourcePointer, NULL);
    glCompileShader(ShaderID);

    //Error Check
    glGetShaderiv(ShaderID, GL_COMPILE_STATUS, &Result);
    glGetShaderiv(ShaderID, GL_INFO_LOG_LENGTH, &InfoLogLength);
    if (InfoLogLength > 0) {
        std::vector<char> ShaderErrorMessage(InfoLogLength + 1);
        glGetShaderInfoLog(ShaderID, InfoLogLength, NULL, &ShaderErrorMessage[0]);
        std::cerr << &ShaderErrorMessage[0];
        throw std::runtime_error("Could not compile shader source code.");
    }

    return ShaderID;
}

GLuint LoadShaders(const std::string& vertex_file_path, const std::string& fragment_file_path){
    GLuint VertexShaderID = CompileShader(vertex_file_path, GL_VERTEX_SHADER);
    GLuint FragmentShaderID = CompileShader(fragment_file_path, GL_FRAGMENT_SHADER);

    GLint Result = GL_FALSE;
    int InfoLogLength;

    //Link Shaders
    std::clog << "Linking program\n";
    GLuint ProgramID = glCreateProgram();
    glAttachShader(ProgramID, VertexShaderID);
    glAttachShader(ProgramID, FragmentShaderID);
    glLinkProgram(ProgramID);

    //Error Check
    glGetProgramiv(ProgramID, GL_LINK_STATUS, &Result);
    glGetProgramiv(ProgramID, GL_INFO_LOG_LENGTH, &InfoLogLength);
    if (InfoLogLength > 0) {
        std::vector<char> ProgramErrorMessage(InfoLogLength + 1);
        glGetProgramInfoLog(ProgramID, InfoLogLength, NULL, &ProgramErrorMessage[0]);
        std::cerr << &ProgramErrorMessage[0] << "\n";
    }

    //Cleanup
    glDetachShader(ProgramID, VertexShaderID);
    glDetachShader(ProgramID, FragmentShaderID);

    glDeleteShader(VertexShaderID);
    glDeleteShader(FragmentShaderID);

    std::clog << "Done loading shaders\n";
    return ProgramID;
}
