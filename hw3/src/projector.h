#pragma once
#ifndef HW3_PROJECTOR_H
#define HW3_PROJECTOR_H

#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtc/type_ptr.hpp>

struct Projector {
    glm::vec3 position;
    glm::vec3 direction;
    float angle;
    float velocity = 0.03f;

    Projector() :
            position(glm::radians(30.0f)), direction(glm::vec3(2, -0.5f, 0)), angle(glm::radians(30.0f)) {}

    void move() {
        glm::mat4 rotationProjector(1);
        rotationProjector = glm::rotate(rotationProjector, velocity, glm::vec3(0.0, 1.0, 0.0));
        direction = glm::vec3(rotationProjector * glm::vec4(direction, 1.0));
    }
};

#endif //HW3_PROJECTOR_H
