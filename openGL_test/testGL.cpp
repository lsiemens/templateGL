#include <vector>
#include <cmath>

#include <iostream>
#include <sstream>
#include <fstream>
#include <stdexcept>

// keep this before all other OpenGL libraries
#define DLEW_STATIC
#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>
using namespace glm;

class FrameTimer {
// Call timer() once in the render loop preferably at the begining or end.
public:
    virtual ~FrameTimer() = default;
    virtual double timer() = 0;
    virtual double getTime() const = 0;
};

class BasicTimer : public FrameTimer {
private:
    double time, previous_time, previous_update;
    unsigned int frame_count;
public:
    BasicTimer() {
        time = glfwGetTime();
        previous_time = previous_update = time;
        frame_count = 0;
    }

    double timer() override {
        frame_count++;
        previous_time = time;
        time = glfwGetTime();

        if (time - previous_update >= 1.0f) {
            std::clog << "\rFPS: " << (double)frame_count/(time - previous_update) << "   ";
            std::clog.flush();
            previous_update = time;
            frame_count = 0;
        }

        return time - previous_time;
    }

    double getTime() const override {
        return time;
    }
};

class AdvancedTimer : public FrameTimer {
private:
    double time, previous_time, previous_update;
    double min_dt, max_dt, mean_dt, previous_mean_dt;
    double current_M2, previous_M2;

    unsigned int frame_count;

    void resetWelford() {
        min_dt = INFINITY, max_dt = -INFINITY, mean_dt = previous_mean_dt = 0;
        current_M2 = previous_M2 = 0;
        frame_count = 0;
    }
public:
    AdvancedTimer() {
        time = glfwGetTime();
        previous_time = previous_update = time;
        resetWelford();
    }

    double timer() override {
        frame_count++;
        previous_time = time;
        previous_mean_dt = mean_dt;
        previous_M2 = current_M2;
        time = glfwGetTime();

        //Welford' online algorith for variance
        double dt = time - previous_time;
        mean_dt = previous_mean_dt + (dt - previous_mean_dt)/frame_count;
        current_M2 = previous_M2 + (dt - previous_mean_dt)*(dt - mean_dt);

        if (dt < min_dt) {
            min_dt = dt;
        }
        if (dt > max_dt) {
            max_dt = dt;
        }

        if (time - previous_update >= 1.0f) {
            std::clog << "FPS: " << (double)frame_count/(time - previous_update) << "   \n";
            std::clog << "Delta t (in ms) : " << 1000*mean_dt << " ± " << 1000*std::sqrt(current_M2/(frame_count - 1))  << " [Min|Max]: [" << 1000*min_dt << " | " << 1000*max_dt << "]   \n";
            std::clog << "\033[A\033[A"; // Move cursor back up two lines
            std::clog.flush();
            previous_update = time;
            resetWelford();
        }

        return time - previous_time;
    }

    double getTime() const override {
        return time;
    }
};

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
    int width=1024, height=768;
    window = glfwCreateWindow( width, height, "GLTest", NULL, NULL);
    if (window == NULL) {
        std::cerr << "Failed to create GLFW window.\n";
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);
    //glfwSwapInterval(0);
    if (glewInit() != GLEW_OK) {
        std::cerr << "Failed to initalize GLEW.\n";
        return -1;
    }

    glClearColor(.6f, .65f, .7f, 1.f);
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);

    glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_TRUE);

    GLuint VertexArrayID;
    glGenVertexArrays(1, &VertexArrayID);
    glBindVertexArray(VertexArrayID);

    static const GLfloat g_vertex_buffer_data[] = {
        -1.0f,-1.0f,-1.0f,
        -1.0f,-1.0f, 1.0f,
        -1.0f, 1.0f, 1.0f,
         1.0f, 1.0f,-1.0f,
        -1.0f,-1.0f,-1.0f,
        -1.0f, 1.0f,-1.0f,
         1.0f,-1.0f, 1.0f,
        -1.0f,-1.0f,-1.0f,
         1.0f,-1.0f,-1.0f,
         1.0f, 1.0f,-1.0f,
         1.0f,-1.0f,-1.0f,
        -1.0f,-1.0f,-1.0f,
        -1.0f,-1.0f,-1.0f,
        -1.0f, 1.0f, 1.0f,
        -1.0f, 1.0f,-1.0f,
         1.0f,-1.0f, 1.0f,
        -1.0f,-1.0f, 1.0f,
        -1.0f,-1.0f,-1.0f,
        -1.0f, 1.0f, 1.0f,
        -1.0f,-1.0f, 1.0f,
         1.0f,-1.0f, 1.0f,
         1.0f, 1.0f, 1.0f,
         1.0f,-1.0f,-1.0f,
         1.0f, 1.0f,-1.0f,
         1.0f,-1.0f,-1.0f,
         1.0f, 1.0f, 1.0f,
         1.0f,-1.0f, 1.0f,
         1.0f, 1.0f, 1.0f,
         1.0f, 1.0f,-1.0f,
        -1.0f, 1.0f,-1.0f,
         1.0f, 1.0f, 1.0f,
        -1.0f, 1.0f,-1.0f,
        -1.0f, 1.0f, 1.0f,
         1.0f, 1.0f, 1.0f,
        -1.0f, 1.0f, 1.0f,
         1.0f,-1.0f, 1.0f
    };

    GLuint vertexbuffer;
    glGenBuffers(1, &vertexbuffer);
    glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(g_vertex_buffer_data), g_vertex_buffer_data, GL_STATIC_DRAW);

    static const GLfloat g_color_buffer_data[] = {
        0.583f,  0.771f,  0.014f,
        0.609f,  0.115f,  0.436f,
        0.327f,  0.483f,  0.844f,
        0.822f,  0.569f,  0.201f,
        0.435f,  0.602f,  0.223f,
        0.310f,  0.747f,  0.185f,
        0.597f,  0.770f,  0.761f,
        0.559f,  0.436f,  0.730f,
        0.359f,  0.583f,  0.152f,
        0.483f,  0.596f,  0.789f,
        0.559f,  0.861f,  0.639f,
        0.195f,  0.548f,  0.859f,
        0.014f,  0.184f,  0.576f,
        0.771f,  0.328f,  0.970f,
        0.406f,  0.615f,  0.116f,
        0.676f,  0.977f,  0.133f,
        0.971f,  0.572f,  0.833f,
        0.140f,  0.616f,  0.489f,
        0.997f,  0.513f,  0.064f,
        0.945f,  0.719f,  0.592f,
        0.543f,  0.021f,  0.978f,
        0.279f,  0.317f,  0.505f,
        0.167f,  0.620f,  0.077f,
        0.347f,  0.857f,  0.137f,
        0.055f,  0.953f,  0.042f,
        0.714f,  0.505f,  0.345f,
        0.783f,  0.290f,  0.734f,
        0.722f,  0.645f,  0.174f,
        0.302f,  0.455f,  0.848f,
        0.225f,  0.587f,  0.040f,
        0.517f,  0.713f,  0.338f,
        0.053f,  0.959f,  0.120f,
        0.393f,  0.621f,  0.362f,
        0.673f,  0.211f,  0.457f,
        0.820f,  0.883f,  0.371f,
        0.982f,  0.099f,  0.879f
    };

    GLuint colorbuffer;
    glGenBuffers(1, &colorbuffer);
    glBindBuffer(GL_ARRAY_BUFFER, colorbuffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(g_color_buffer_data), g_color_buffer_data, GL_STATIC_DRAW);

    GLuint programID = LoadShaders("vertex.shader", "fragment.shader");

    GLuint MatrixID = glGetUniformLocation(programID, "MVP");

    AdvancedTimer timer = AdvancedTimer();
    double dt;
    float theta = 0;
    do {
        // Timing
        dt = timer.timer();

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        //Camera

        theta += 2.0*dt;
        glm::mat4 Projection = glm::perspective(glm::radians(45.f), (float)width/(float)height, 0.1f, 100.f);
        //glm::mat4 Projection = glm::ortho(-10.f, 10.f, -10.f, 10.f, 0.f, 100.f);
        //glm::mat4 View = glm::lookAt(glm::vec3(4, 3, 3), glm::vec3(0, 0, 0), glm::vec3(0, 1, 0));
        glm::mat4 View = glm::lookAt(glm::vec3(4*std::cos(theta), 3, 4*std::sin(theta)), glm::vec3(0, 0, 0), glm::vec3(0, 1, 0));
        glm::mat4 Model = glm::mat4(1.f);

        glm::mat4 mvp = Projection*View*Model;

        glUniformMatrix4fv(MatrixID, 1, GL_FALSE, &mvp[0][0]);

        //Draw
        glUseProgram(programID);
        glClear(GL_COLOR_BUFFER_BIT);

        glEnableVertexAttribArray(0);
        glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
        glDrawArrays(GL_TRIANGLES, 0, 12*3);
        glDisableVertexAttribArray(0);

        glEnableVertexAttribArray(1);
        glBindBuffer(GL_ARRAY_BUFFER, colorbuffer);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
        glDisableVertexAttribArray(0);

        //Draw end

        glfwSwapBuffers(window);
        glfwPollEvents();
    } while ((glfwGetKey(window, GLFW_KEY_ESCAPE) != GLFW_PRESS) && (glfwWindowShouldClose(window) == 0));
    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}
