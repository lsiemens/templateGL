#include "object.h"

Object::Object(Model model, glm::vec3 velocity) : model(model), velocity(velocity) {
    modelSpaceToWorldSpace = glm::mat4(1.f);
}

void Object::drawObject() {
    this->model.drawModel(modelSpaceToWorldSpace);
}

void Object::update(double dt) {
    modelSpaceToWorldSpace = glm::translate(modelSpaceToWorldSpace, (float)dt*velocity);
}
