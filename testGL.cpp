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
#include "frame_timer.h"
#include "model.h"
#include "object.h"
#include "camera.h"
#include "shaders.h"

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
