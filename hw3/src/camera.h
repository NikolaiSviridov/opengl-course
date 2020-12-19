#pragma once
#ifndef HW3_CAMERA_H
#define HW3_CAMERA_H

class Camera {
    float velocity = 0.05;
    float cameraRotationUpVelocity = 0.04;

public:
    glm::vec3 position;
    glm::vec3 direction;
    glm::vec3 up{};

    Camera() :
            position(glm::vec3(-10.8, 2.285, -2.24)), direction(glm::vec3(1.18902, -0.405856, -0.258205)) {}

    void keyboard_callback(GLFWwindow *window) {
        if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
            position += direction * velocity;
        if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
            glm::mat4 rotationMat(1);
            rotationMat = glm::rotate(rotationMat, glm::radians(90.0f), glm::vec3(0.0, 1.0, 0.0));
            glm::vec3 left = glm::vec3(rotationMat * glm::vec4(direction, 1.0));
            left.y = 0;
            position += left * velocity;
        }
        if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
            position -= direction * velocity;
        if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
            glm::mat4 rotationMat(1);
            rotationMat = glm::rotate(rotationMat, glm::radians(-90.0f), glm::vec3(0.0, 1.0, 0.0));
            glm::vec3 right = glm::vec3(rotationMat * glm::vec4(direction, 1.0));
            right.y = 0;
            position += right * velocity;
        }
        if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS) {
            glm::mat4 rotationMat(1);
            rotationMat = glm::rotate(rotationMat, velocity, glm::vec3(0.0, 1.0, 0.0));
            direction = glm::vec3(rotationMat * glm::vec4(direction, 1.0));
        }
        if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS) {
            glm::mat4 rotationMat(1);
            rotationMat = glm::rotate(rotationMat, -velocity, glm::vec3(0.0, 1.0, 0.0));
            direction = glm::vec3(rotationMat * glm::vec4(direction, 1.0));
        }
        if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS) {
            glm::mat4 rotationMat(1);
            rotationMat = glm::rotate(rotationMat, cameraRotationUpVelocity,
                                      glm::cross(direction, glm::vec3(0.0, 1.0, 0.0)));
            glm::vec3 newDir = glm::vec3(rotationMat * glm::vec4(direction, 1.0));
            if (glm::abs(glm::sin(glm::acos(glm::dot(glm::normalize(newDir), glm::vec3(0.0, 1.0, 0.0))))) > 0.1) {
                direction = newDir;
            }
        }
        if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS) {
            glm::mat4 rotationMat(1);
            rotationMat = glm::rotate(rotationMat, -cameraRotationUpVelocity,
                                      glm::cross(direction, glm::vec3(0.0, 1.0, 0.0)));
            glm::vec3 newDir = glm::vec3(rotationMat * glm::vec4(direction, 1.0));
            if (glm::abs(glm::sin(glm::acos(glm::dot(glm::normalize(newDir), glm::vec3(0.0, 1.0, 0.0))))) > 0.1) {
                direction = newDir;
            }
        }
    }
};

#endif
