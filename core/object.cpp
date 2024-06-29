#include "object.h"

Object::Object(Model model) : model(model) {
    modelSpaceToWorldSpace = glm::translate(glm::mat4(1.f), position);
}

void Object::drawObject() {
    this->model.drawModel(modelSpaceToWorldSpace);
}

void Object::update(double dt) {
    velocity += 0.5f*(float)dt*acceleration;
    position += (float)dt*velocity;
    velocity += 0.5f*(float)dt*acceleration;
    modelSpaceToWorldSpace = glm::translate(glm::mat4(1.0f), position);

    // fake a floor
    if ((position.y < 0) && (velocity.y*position.y > 0)) {
        velocity.y *= -0.9;
    }
}
