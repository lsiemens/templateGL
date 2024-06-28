#ifndef OBJECT_H
#define OBJECT_H

#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>

#include "model.h"

/// A class for defineing objects using a given model.
class Object {
public:
    Model model;
    glm::vec3 velocity;
    glm::mat4 modelSpaceToWorldSpace;

    Object(Model model, glm::vec3 velocity=glm::vec3());
    void drawObject();
    void update(double dt);
};

#endif
