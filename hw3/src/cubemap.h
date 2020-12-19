#pragma once

#include "opengl_shader.h"
#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "../includes/stb_image.h"
#include "iostream"

class Cubemap {
    unsigned int texture{};
    unsigned int VAO{};
    shader_t shader;
public:
    Cubemap() = default;

    Cubemap(const std::vector<std::string> &faces,
            const shader_t &shader) :
            shader(shader), texture(load_texture(faces)), VAO(load_vertices(100.0)) {}

    static unsigned int load_texture(std::vector<std::string> faces) {
        unsigned int textureID;
        glGenTextures(1, &textureID);
        glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);

        int width, height, nrChannels;
        for (unsigned int i = 0; i < faces.size(); i++) {
            unsigned char *data = stbi_load(faces[i].c_str(), &width, &height, &nrChannels, 0);
            if (data) {
                glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i,
                             0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data
                );
                stbi_image_free(data);
            } else {
                std::cout << "Cubemap tex failed to load at path: " << faces[i] << std::endl;
                stbi_image_free(data);
            }
        }

        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

        return textureID;
    }

    static unsigned int load_vertices(float scale) {
        float cubeVertices[] = {
                -scale, scale, -scale,
                -scale, -scale, -scale,
                scale, -scale, -scale,
                scale, -scale, -scale,
                scale, scale, -scale,
                -scale, scale, -scale,

                -scale, -scale, scale,
                -scale, -scale, -scale,
                -scale, scale, -scale,
                -scale, scale, -scale,
                -scale, scale, scale,
                -scale, -scale, scale,

                scale, -scale, -scale,
                scale, -scale, scale,
                scale, scale, scale,
                scale, scale, scale,
                scale, scale, -scale,
                scale, -scale, -scale,

                -scale, -scale, scale,
                -scale, scale, scale,
                scale, scale, scale,
                scale, scale, scale,
                scale, -scale, scale,
                -scale, -scale, scale,

                -scale, scale, -scale,
                scale, scale, -scale,
                scale, scale, scale,
                scale, scale, scale,
                -scale, scale, scale,
                -scale, scale, -scale,

                -scale, -scale, -scale,
                -scale, -scale, scale,
                scale, -scale, -scale,
                scale, -scale, -scale,
                -scale, -scale, scale,
                scale, -scale, scale
        };

        unsigned int VBO, VAO;

        glGenVertexArrays(1, &VAO);
        glGenBuffers(1, &VBO);
        glBindVertexArray(VAO);
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(cubeVertices), &cubeVertices[0], GL_STATIC_DRAW);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void *) 0);

        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindVertexArray(0);

        return VAO;
    }

    void draw(glm::mat4 vp) {
        shader.use();
        shader.set_uniform("VP", glm::value_ptr(vp));
        glBindVertexArray(VAO);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_CUBE_MAP, texture);
        glDrawArrays(GL_TRIANGLES, 0, 36);
        glBindVertexArray(0);
    }
};
