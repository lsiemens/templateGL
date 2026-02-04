#ifndef OBJECT_H
#define OBJECT_H

#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>

#include "core/model.h"

/// A class for defining objects using a given model.
class Object {
public:
    Model model;
    float mass=1.f;
    glm::vec3 position = glm::vec3(0.0f);
    glm::vec3 velocity = glm::vec3(0.0f);
    glm::vec3 acceleration = glm::vec3(0.0f);

    glm::mat4 modelSpaceToWorldSpace;

    Object(Model model);
    void drawObject();
    void update(float dt);
};

#endif
