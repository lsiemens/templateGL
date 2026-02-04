#include <cmath>
#include <iostream>

// keep this before all other OpenGL libraries
#define GLEW_STATIC
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

#include "core/looplog.h"
#include "core/frame_timer.h"
#include "core/model.h"
#include "core/object.h"
#include "core/camera.h"
#include "core/shaders.h"
#include "core/path_util.h"

void Controlls(float dt, GLFWwindow* window, Camera &camera) {
    double horizontalAngle = 3.13, verticalAngle = 0.0;
    float speed = 3.f, mouseSensitivity = 0.001f;

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
        delta_position = dt*direction*speed;
    }
    if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS) {
        delta_position = -dt*direction*speed;
    }
    if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS) {
        delta_position = dt*right*speed;
    }
    if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS) {
        delta_position = -dt*right*speed;
    }
    if (glfwGetKey(window, GLFW_KEY_ENTER) == GLFW_PRESS) {
        delta_position = dt*up*speed;
    }
    if (glfwGetKey(window, GLFW_KEY_RIGHT_SHIFT) == GLFW_PRESS) {
        delta_position = -dt*up*speed;
    }

    camera.m_direction = direction;
    camera.m_up = up;
    camera.m_position += delta_position;
}

glm::vec2 meshgrid(int x_resolution, int y_resolution, int vertex_index) {
    int triangle_index = vertex_index / 3; // index of the triangle
    int triangle_vertex_index = vertex_index % 3; // label of the vertex within the triangle

    int quad_index = triangle_index / 2; // index of the quad
    int quad_triangle_index = triangle_index % 2; // label of the triangle within the quad

    int column_index = quad_index / (x_resolution - 1); // index of the column
    int row_index = quad_index % (x_resolution - 1); // index of the row

    // lower left coordinate of triangle on the unit square
    glm::vec2 unit_pos;
    unit_pos.x = static_cast<float>(row_index)/static_cast<float>(x_resolution - 1);
    unit_pos.y = static_cast<float>(column_index)/static_cast<float>(y_resolution - 1);

    // offset for the upper right vertex
    if (triangle_vertex_index == 1) {
        unit_pos.x += 1.0f/static_cast<float>(x_resolution - 1);
        unit_pos.y += 1.0f/static_cast<float>(y_resolution - 1);
    }

    // offsets for the off diagonal vertex. The spesific offset
    // is dependent on if it is the upper or lower triangle
    if (triangle_vertex_index == 2){
        if (quad_triangle_index == 0) {
            unit_pos.x += 1.0f/static_cast<float>(x_resolution - 1);
        } else {
            unit_pos.y += 1.0f/static_cast<float>(y_resolution - 1);
        }
    }
    return unit_pos;
}

Object initalizeSurface(GLuint shaderID) {
    constexpr int x_resolution = 100;
    constexpr int y_resolution = 100;
    constexpr int num_vertices = (x_resolution - 1)*(y_resolution - 1)*2*3; // (x_resolution - 1)*(y_resolution - 1) quads, 2 triangles per quad, 3 points per triangle

    GLfloat g_vertex_buffer_data[num_vertices*3];
    GLfloat g_color_buffer_data[num_vertices*3];

    for (int vertex_index = 0; vertex_index < num_vertices; vertex_index++) {
        glm::vec2 unit_pos = meshgrid(x_resolution, y_resolution, vertex_index);

        float x = 5*(unit_pos.x - 0.5f);
        float y = 5*(unit_pos.y - 0.5f);

        g_vertex_buffer_data[0 + 3*vertex_index] = x;
        g_vertex_buffer_data[2 + 3*vertex_index] = y;
        g_vertex_buffer_data[1 + 3*vertex_index] = std::exp(-(x*x + y*y));

        g_color_buffer_data[0 + 3*vertex_index] = unit_pos.x;
        g_color_buffer_data[1 + 3*vertex_index] = unit_pos.y;
        g_color_buffer_data[2 + 3*vertex_index] = std::exp(-(x*x + y*y));
    }

    Model model = Model(shaderID);
    model.setVertexBuffer(g_vertex_buffer_data, sizeof(g_vertex_buffer_data));
    model.setColorBuffer(g_color_buffer_data, sizeof(g_color_buffer_data));
    Object surface = Object(model);
    return surface;
}

Object initalizeSphere(GLuint shaderID) {
    constexpr int x_resolution = 32; // rows
    constexpr int y_resolution = 16; // columns
    constexpr int num_vertices = (x_resolution - 1)*(y_resolution - 1)*2*3; // (x_resolution - 1)*(y_resolution - 1) quads, 2 triangles per quad, 3 points per triangle

    GLfloat g_vertex_buffer_data[num_vertices*3];
    GLfloat g_color_buffer_data[num_vertices*3];

    for (int vertex_index = 0; vertex_index < num_vertices; vertex_index++) {
        glm::vec2 unit_pos = meshgrid(x_resolution, y_resolution, vertex_index);

        float theta = 6.28f*unit_pos.x;
        float phi = 3.14f*unit_pos.y;

        float x = std::sin(phi)*std::cos(theta);
        float y = std::sin(phi)*std::sin(theta);
        float z = std::cos(phi);

        g_vertex_buffer_data[0 + 3*vertex_index] = x;
        g_vertex_buffer_data[2 + 3*vertex_index] = y;
        g_vertex_buffer_data[1 + 3*vertex_index] = z;

        g_color_buffer_data[0 + 3*vertex_index] = unit_pos.x;
        g_color_buffer_data[1 + 3*vertex_index] = unit_pos.y;
        g_color_buffer_data[2 + 3*vertex_index] = 0.0f;
    }

    Model model = Model(shaderID);
    model.setVertexBuffer(g_vertex_buffer_data, sizeof(g_vertex_buffer_data));
    model.setColorBuffer(g_color_buffer_data, sizeof(g_color_buffer_data));
    Object sphere = Object(model);
    return sphere;
}

Object initalizeTorus(GLuint shaderID) {
    constexpr int x_resolution = 100; // rows
    constexpr int y_resolution = 100; // columns
    constexpr int num_vertices = (x_resolution - 1)*(y_resolution - 1)*2*3; // (x_resolution - 1)*(y_resolution - 1) quads, 2 triangles per quad, 3 points per triangle

    GLfloat g_vertex_buffer_data[num_vertices*3];
    GLfloat g_color_buffer_data[num_vertices*3];

    for (int vertex_index = 0; vertex_index < num_vertices; vertex_index++) {
        glm::vec2 unit_pos = meshgrid(x_resolution, y_resolution, vertex_index);

        float theta = 6.28f*unit_pos.x;
        float phi = 6.28f*unit_pos.y;

        float x = (1.0f + 0.5f*std::cos(theta))*std::cos(phi);
        float y = (1.0f + 0.5f*std::cos(theta))*std::sin(phi);
        float z = 0.5f*std::sin(theta);

        g_vertex_buffer_data[0 + 3*vertex_index] = x;
        g_vertex_buffer_data[2 + 3*vertex_index] = y;
        g_vertex_buffer_data[1 + 3*vertex_index] = z;

        g_color_buffer_data[0 + 3*vertex_index] = unit_pos.x;
        g_color_buffer_data[1 + 3*vertex_index] = unit_pos.y;
        g_color_buffer_data[2 + 3*vertex_index] = 0.0f;
    }

    Model model = Model(shaderID);
    model.setVertexBuffer(g_vertex_buffer_data, sizeof(g_vertex_buffer_data));
    model.setColorBuffer(g_color_buffer_data, sizeof(g_color_buffer_data));
    Object torus = Object(model);
    return torus;
}

int main() {
    LoopLog* loopLog = LoopLog::getInstance();
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
    GLuint shaderID = LoadShaders("assets/vertex.glsl", "assets/fragment.glsl");

    AdvancedTimer timer = AdvancedTimer();
    Camera camera = Camera(shaderID);
    Object surface = initalizeSurface(shaderID);

    Object sphere = initalizeSphere(shaderID);
    sphere.m_position = glm::vec3(3.0f, 0.0f, -3.0f);
    sphere.m_velocity = glm::vec3(0.0f, 10.0f, 0.0f);
    sphere.m_acceleration = glm::vec3(0.0f, -9.81f, 0.0f);

    Object torus = initalizeTorus(shaderID);
    torus.m_position = glm::vec3(-3.0f, 0.0f, -3.0f);
    float dt;
    do {
        // Timing
        dt = static_cast<float>(timer.timer());
        loopLog->flush();

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        //Camera
        Controlls(dt, window, camera);

        glUseProgram(shaderID);
        camera.update();

        surface.update(dt);
        sphere.update(dt);
        torus.update(dt);

        surface.drawObject();
        sphere.drawObject();
        torus.drawObject();

        glfwSwapBuffers(window);
        glfwPollEvents();
    } while ((glfwGetKey(window, GLFW_KEY_ESCAPE) != GLFW_PRESS) && (glfwWindowShouldClose(window) == 0));
    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}
