#ifndef OBJECT_H
#define OBJECT_H

#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>

#include "core/model.h"

/// A class for defining objects using a given model.
class Object {
public:
    Model m_model;
    float m_mass=1.f;
    glm::vec3 m_position = glm::vec3(0.0f);
    glm::vec3 m_velocity = glm::vec3(0.0f);
    glm::vec3 m_acceleration = glm::vec3(0.0f);

    glm::mat4 m_modelSpaceToWorldSpace;

    Object(Model model);
    void drawObject();
    void update(float dt);
};

#endif
