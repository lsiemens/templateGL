#include "core/object.h"

Object::Object(Model model) : m_model(model) {
    m_modelSpaceToWorldSpace = glm::translate(glm::mat4(1.f), m_position);
}

void Object::drawObject() {
    this->m_model.drawModel(m_modelSpaceToWorldSpace);
}

void Object::update(float dt) {
    m_velocity += 0.5f*dt*m_acceleration;
    m_position += dt*m_velocity;
    m_velocity += 0.5f*dt*m_acceleration;
    m_modelSpaceToWorldSpace = glm::translate(glm::mat4(1.0f), m_position);

    // fake a floor
    if ((m_position.y < 0) && (m_velocity.y*m_position.y > 0)) {
        m_velocity.y *= -0.9f;
    }
}
