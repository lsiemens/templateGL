#include <vector>
#include <cmath>
#include <random>

#include <iostream>
#include <fstream>
#include <stdexcept>
#include <string>

// keep this before all other OpenGL libraries
#define GLEW_STATIC
#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>

#include "looplog.h"

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
    LoopLog* loopLog;
public:
    BasicTimer() {
        time = glfwGetTime();
        previous_time = previous_update = time;
        frame_count = 0;

        loopLog = LoopLog::getInstance();
    }

    double timer() override {
        frame_count++;
        previous_time = time;
        time = glfwGetTime();

        if (time - previous_update >= 1.0f) {
            loopLog->log << "FPS: " << (double)frame_count/(time - previous_update) << "\n";
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
    LoopLog* loopLog;

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

        loopLog = LoopLog::getInstance();
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
            loopLog->log << "FPS: " << (double)frame_count/(time - previous_update) << "\n";
            loopLog->log << "Delta t (in ms) : " << 1000*mean_dt << " ± " << 1000*std::sqrt(current_M2/(frame_count - 1))  << " [Min|Max]: [" << 1000*min_dt << " | " << 1000*max_dt << "]\n";
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

class Model {
public:
    GLuint vertexbuffer;
    GLuint colorbuffer;
    unsigned int vertexBufferSize;
    GLuint matrixID;

    Model(GLuint shaderID) {
        matrixID = glGetUniformLocation(shaderID, "ModelTransform");
    }

    void releaseBuffers() {
        glDeleteBuffers(1, &vertexbuffer);
        glDeleteBuffers(1, &colorbuffer);
    }

    void setVertexBuffer(GLfloat data[], unsigned int bufferSize) {
        vertexBufferSize = bufferSize;
        glGenBuffers(1, &vertexbuffer);
        glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
        glBufferData(GL_ARRAY_BUFFER, bufferSize, data, GL_STATIC_DRAW);
    }

    void setColorBuffer(GLfloat data[], unsigned int bufferSize) {
        glGenBuffers(1, &colorbuffer);
        glBindBuffer(GL_ARRAY_BUFFER, colorbuffer);
        glBufferData(GL_ARRAY_BUFFER, bufferSize, data, GL_STATIC_DRAW);
    }

    void drawModel(glm::mat4 modelSpaceToWorldSpace) {
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
};

class Object {
public:
    glm::mat4 modelSpaceToWorldSpace;
    glm::vec3 velocity;
    Model model;

    Object(Model model, glm::vec3 velocity=glm::vec3()) : model(model), velocity(velocity) {
        modelSpaceToWorldSpace = glm::mat4(1.f);
    }

    void drawObject() {
        this->model.drawModel(modelSpaceToWorldSpace);
    }

    void update(double dt) {
        modelSpaceToWorldSpace = glm::translate(modelSpaceToWorldSpace, (float)dt*velocity);
    }
};

class Camera {
public:
    glm::vec3 position;
    glm::vec3 direction;
    glm::vec3 up;
    double FoV, aspectRatio;
    GLuint matrixID;

    Camera(GLuint shaderID) {
        matrixID = glGetUniformLocation(shaderID, "CameraTransform");
        position = glm::vec3(0, 0, -12);
        direction = glm::vec3(1, 0, 0);
        up = glm::vec3(0, 1, 0);
        FoV = 90.f;
        aspectRatio = 4.f/3.f;
    }

    glm::mat4 getViewMatrix() {
        return glm::lookAt(position, position + direction, up);
    }

    glm::mat4 getProjectionMatrix() {
        return glm::perspective((float)glm::radians(FoV), (float)aspectRatio, 0.1f, 100.f);
        //return glm::ortho(-10.f, 10.f, -10.f, 10.f, 0.f, 100.f);
    }

    void update() {
        glm::mat4 matrix = getProjectionMatrix()*getViewMatrix();
        glUniformMatrix4fv(matrixID, 1, GL_FALSE, &matrix[0][0]);
    }
};

void Controlls(double dt, GLFWwindow* window, Camera &camera) {
    double horizontalAngle = 3.13f, verticalAngle = 0.f;
    double speed = 3.f, mouseSensitivity = 0.001f;

    double xpos, ypos;
    glfwGetCursorPos(window, &xpos, &ypos);
    horizontalAngle = -mouseSensitivity*xpos;
    verticalAngle = mouseSensitivity*ypos;

    glm::vec3 direction = glm::vec3(std::cos(verticalAngle)*std::sin(horizontalAngle),
                     std::sin(verticalAngle),
                     std::cos(verticalAngle)*std::cos(horizontalAngle));

    glm::vec3 right = glm::vec3(std::sin(horizontalAngle - 3.14f/2.f),
                     0.f,
                     std::cos(horizontalAngle - 3.14/2.f));

    glm::vec3 up = glm::cross(right, direction);
    glm::vec3 delta_position = glm::vec3(0.f, 0.f, 0.f);

    if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS) {
        delta_position = (float)dt*direction*(float)speed;
    }
    if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS) {
        delta_position = -(float)dt*direction*(float)speed;
    }
    if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS) {
        delta_position = (float)dt*right*(float)speed;
    }
    if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS) {
        delta_position = -(float)dt*right*(float)speed;
    }
    if (glfwGetKey(window, GLFW_KEY_ENTER) == GLFW_PRESS) {
        delta_position = (float)dt*up*(float)speed;
    }
    if (glfwGetKey(window, GLFW_KEY_RIGHT_SHIFT) == GLFW_PRESS) {
        delta_position = -(float)dt*up*(float)speed;
    }

    camera.direction = direction;
    camera.up = up;
    camera.position += delta_position;
    LoopLog* loopLog = LoopLog::getInstance();
    loopLog->log << "Camera " << camera.position.x << " " << camera.position.y << " " << camera.position.z << "\n";
}

Object initalizeCube(GLuint shaderID) {
    GLfloat g_vertex_buffer_data[] = {
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

    GLfloat g_color_buffer_data[12*3];

    for (int i=0; i < std::size(g_color_buffer_data); i++) {
        g_color_buffer_data[i] = static_cast <GLfloat> (rand()) / static_cast<float> (RAND_MAX);
    }

    Model model = Model(shaderID);
    model.setVertexBuffer(g_vertex_buffer_data, sizeof(g_vertex_buffer_data));
    model.setColorBuffer(g_color_buffer_data, sizeof(g_color_buffer_data));
    Object cube = Object(model);
    return cube;
}

Object initalizeSurface(GLuint shaderID) {
    float t=0, s=0;
    int n = 100;
    int verticii = (n - 1)*(n - 1)*2*3; // (n - 1)^2 quads, 2 triangles per quad, 3 points per triangle

    GLfloat g_vertex_buffer_data[verticii*3];
    GLfloat g_color_buffer_data[verticii*3];

    for (int i = 0; i < n - 1; i++) {
        for (int j = 0; j < n - 1; j++) {
            int qindex = i + (n - 1)*j;
            for (int k=0; k<2; k++) {
                int tindex = k + 2*qindex;
                for (int m = 0; m < 3; m++) {
                    int vindex = m + 3*tindex;

                    int i_p = i;
                    int j_p = j;
                    if (m == 1) {
                        i_p++;
                        j_p++;
                    }
                    if (m == 2) {
                        if (k == 0) {
                            i_p++;
                        } else {
                            j_p++;
                        }
                    }
                    float ux = (float)(i_p)/n;
                    float uy = (float)(j_p)/n;
                    float x = 5*(ux - 0.5);
                    float y = 5*(uy - 0.5);
                    g_vertex_buffer_data[3*vindex] = x;
                    g_vertex_buffer_data[1 + 3*vindex] = y;
                    g_vertex_buffer_data[2 + 3*vindex] = std::exp(-x*x - y*y);

                    g_color_buffer_data[3*vindex] = ux;
                    g_color_buffer_data[1 + 3*vindex] = uy;
                    g_color_buffer_data[2 + 3*vindex] = std::exp(-x*x - y*y);
                }
            }
        }
    }

    Model model = Model(shaderID);
    model.setVertexBuffer(g_vertex_buffer_data, sizeof(g_vertex_buffer_data));
    model.setColorBuffer(g_color_buffer_data, sizeof(g_color_buffer_data));
    Object cube = Object(model);
    return cube;
}

int main() {
    LoopLog* loopLog = LoopLog::getInstance();
    srand(time(NULL));
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
    glfwSwapInterval(0);
    if (glewInit() != GLEW_OK) {
        std::cerr << "Failed to initalize GLEW.\n";
        return -1;
    }

    glClearColor(.6f, .65f, .7f, 1.f);
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);

    glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_TRUE);
    //glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    GLuint VertexArrayID;
    glGenVertexArrays(1, &VertexArrayID);
    glBindVertexArray(VertexArrayID);

    // Initalize shader
    GLuint shaderID = LoadShaders("vertex.shader", "fragment.shader");

    AdvancedTimer timer = AdvancedTimer();
    Camera camera = Camera(shaderID);
    Object cube = initalizeCube(shaderID);
    cube.velocity = glm::vec3(1.f, 0.f, 0.f);
    Object cube2 = initalizeCube(shaderID);
    cube2.velocity = glm::vec3(0.f, 2.f, 0.f);
    Object cube3 = initalizeSurface(shaderID);
    double dt;
    do {
        // Timing
        dt = timer.timer();
        loopLog->flush();

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        //Camera
        Controlls(dt, window, camera);

        glUseProgram(shaderID);
        camera.update();

        cube.update(dt);
        cube2.update(dt);
        cube3.update(dt);

        cube.drawObject();
        cube2.drawObject();
        cube3.drawObject();

        glfwSwapBuffers(window);
        glfwPollEvents();
    } while ((glfwGetKey(window, GLFW_KEY_ESCAPE) != GLFW_PRESS) && (glfwWindowShouldClose(window) == 0));
    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}
