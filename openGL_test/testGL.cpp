// Hello world

#include <vector>

#include <iostream>
#include <sstream>
#include <fstream>
#include <stdexcept>

// keep this before all other OpenGL libraries
#define DLEW_STATIC
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
using namespace glm;

GLuint CompileShader(const char* shader_file_path, GLenum shader_type){
    GLuint ShaderID = glCreateShader(shader_type);

    //Get Code
    std::string ShaderCode;
    std::ifstream ShaderStream(shader_file_path, std::ios::in);
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

GLuint LoadShaders(const char* vertex_file_path, const char* fragment_file_path){
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

int main() {
    if (!glfwInit()) {
        std::cerr << "Failed to initalize GLFW\n";
        return -1;
    }

    GLFWwindow* window;
    window = glfwCreateWindow( 1024, 768, "GLTest", NULL, NULL);
    if (window == NULL) {
        std::cerr << "Failed to create GLFW window.\n";
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);
    if (glewInit() != GLEW_OK) {
        std::cerr << "Failed to initalize GLEW.\n";
        return -1;
    }

    glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_TRUE);

    GLuint VertexArrayID;
    glGenVertexArrays(1, &VertexArrayID);
            glBindVertexArray(VertexArrayID);

    static const GLfloat g_vertex_buffer_data[] = {-1.f, -1.f, 0.f,
                                                    1.f, -1.f, 0.f,
                                                    0.f,  1.f, 0.f};
    GLuint vertexbuffer;
    glGenBuffers(1, &vertexbuffer);
    glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(g_vertex_buffer_data), g_vertex_buffer_data, GL_STATIC_DRAW);

    GLuint programID = LoadShaders("vertex.shader", "fragment.shader");
    glClearColor(.6f, .65f, .7f, 1.f);

    do {
        glClear(GL_COLOR_BUFFER_BIT);

        //Draw
        glUseProgram(programID);
        glClear(GL_COLOR_BUFFER_BIT);

        glEnableVertexAttribArray(0);
        glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
        glDrawArrays(GL_TRIANGLES, 0, 3);
        glDisableVertexAttribArray(0);

        //Draw end

        glfwSwapBuffers(window);
        glfwPollEvents();
    } while ((glfwGetKey(window, GLFW_KEY_ESCAPE) != GLFW_PRESS) && (glfwWindowShouldClose(window) == 0));
    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}
