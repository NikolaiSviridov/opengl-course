#pragma once
#ifndef HW3_BOAT_H
#define HW3_BOAT_H

#include <util/model.h>

class Boat : public Model {
    float radius = 8.0;
    glm::vec3 circle_center = glm::vec3(-8, 0, -8);
    glm::vec3 circle_radius_z = glm::vec3(0, 0, radius);
    glm::vec3 circle_radius_x = glm::vec3(radius, 0, 0);
    glm::vec3 circle_start = circle_radius_z;
    float boat_velocity = 0.2f;

public:
    Boat() = default;

    Boat(const std::string &filename, const std::string &basepath, float factor) : Model(filename, basepath, factor) {
        position = circle_center + circle_start;
    }

    void move() {
        glm::mat4 rotationBoat(1);
        rotationBoat = glm::rotate(rotationBoat, glm::radians(boat_velocity),
                                   glm::vec3(0.0, 1.0, 0.0));
        circle_radius_z = glm::vec3(rotationBoat * glm::vec4(circle_radius_z, 1.0));
        boat_rotation = glm::atan(glm::dot(circle_radius_z, circle_radius_x), glm::dot(circle_radius_z, circle_start));
        position = circle_center + circle_radius_z;
    }

    float boat_rotation;
};

#endif
