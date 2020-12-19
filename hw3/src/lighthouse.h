#pragma once
#ifndef HW3_LIGHTHOUSE_H
#define HW3_LIGHTHOUSE_H

#include <util/model.h>

class Lighthouse : public Model {
public:
    Lighthouse() = default;

    Lighthouse(const std::string &filename, const std::string &basepath, float factor) :
            Model(filename, basepath, factor) {
        position = glm::vec3(-6.19, 0.52, -2.2);
    }
};

#endif
