#pragma one
#ifndef HW3_SUN_H
#define HW3_SUN_H

#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtc/type_ptr.hpp>

struct Sun {
    glm::vec4 direction;

    Sun() : direction(glm::vec4(1, 0.5, 1, 0.0)) {}
};

#endif
