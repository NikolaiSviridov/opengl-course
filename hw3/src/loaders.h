#pragma once
#ifndef HW3_LOADERS_H
#define HW3_LOADERS_H

#define TINYOBJLOADER_IMPLEMENTATION

#include "../includes/tiny_obj_loader.h"
#include "../includes/stb_image.h"

#include <fmt/format.h>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include "opengl_shader.h"
#include<iostream>
#include<string>

#include <util/mesh.h>
#include "land.h"

Texture load_tile_tex(const std::string &path) {
    Texture texture;

    unsigned int textureID;
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_2D, textureID);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    int width, height, nrChannels;
    unsigned char *data = stbi_load(path.c_str(), &width, &height, &nrChannels, 0);
    if (data) {
        if (nrChannels == 3) {
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
        } else if (nrChannels == 4) {
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
        }
    } else {
        std::cout << "Texture failed to load at path: " << path << std::endl;
        stbi_image_free(data);
        exit(1);
    }
    stbi_image_free(data);

    glGenerateMipmap(GL_TEXTURE_2D);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    glBindTexture(GL_TEXTURE_2D, 0);

    texture.path = path;
    texture.id = textureID;
    return texture;
}

void load_water(Mesh &water,
                const std::string &normal_path,
                const std::string &dudv_path,
                float scale,
                float density) {
    float planeVertices[] = {
            scale, 0.0f, scale,
            0.0f, density,
            scale, 0.0f, -scale,
            0.0f, 0.0f,
            -scale, 0.0f, -scale,
            density, 0.0f,
            -scale, 0.0f, scale,
            density, density
    };

    unsigned int indices[] = {0, 1, 3, 1, 2, 3};

    unsigned int VBO, VAO, EBO;

    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(planeVertices), planeVertices, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void *) 0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void *) (sizeof(float) * 3));

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    water.VAO = VAO;
    water.IndexCount = 6;

    water.textures.push_back(load_tile_tex(normal_path));
    water.textures.push_back(load_tile_tex(dudv_path));
}


void load_land(Land &landscape,
               const std::string heigh_map,
               const std::string texture1,
               const std::string texture2,
               const std::string texture3
) {
    int width, height, nrChannels;
    unsigned char *data = stbi_load(heigh_map.c_str(), &width, &height, &nrChannels, 0);
    if (data) {
        if (nrChannels == 3) {
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
        } else if (nrChannels == 4) {
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
        }
    } else {
        std::cout << "Landscape tex failed to load at path: " << heigh_map << std::endl;
        stbi_image_free(data);
        exit(1);
    }

    int currentPixel = 0;
    float heightCoefficient = 2;
    for (int i = 0; i < height; i++) {
        std::vector<float> row;
        for (int j = 0; j < width; j++) {
            row.push_back(data[currentPixel] / 255.0f * heightCoefficient);
            currentPixel += nrChannels;
        }
        landscape.height_map.push_back(row);
    }

    stbi_image_free(data);

    std::vector<float> vertices;
    std::vector<unsigned int> indices;
    unsigned int indicesOffset = 0;
    int texture_density = 50;
    float threshold1 = 0.1;
    float threshold2 = 0.95;
    int scale = 14;
    for (int hh = 1; hh <= height - 2; hh += 2) {
        for (int ww = 1; ww <= width - 2; ww += 2) {
            float i = ((float) hh / height - 1) * scale;
            float j = ((float) ww / width - 1) * scale;
            float shiftW = scale * 1.0 / width;
            float shiftH = scale * 1.0 / height;

            int ii = hh;
            int jj = ww;

            float density_f = (float) texture_density;
            float dHPlus = ((hh + 1) % texture_density) / density_f;
            if (dHPlus == 0) {
                dHPlus = 1;
            }
            float dWPlus = ((ww + 1) % texture_density) / density_f;
            if (dWPlus == 0) {
                dWPlus = 1;
            }

            float current[] = {
                    i, landscape.height_map[ii][jj + 1], j + shiftW,
                    (hh % texture_density) / density_f, dWPlus,
                    i + shiftH, landscape.height_map[ii + 1][jj + 1], j + shiftW,
                    dHPlus, dWPlus,
                    i, landscape.height_map[ii][jj], j,
                    (hh % texture_density) / density_f, (ww % texture_density) / density_f,
                    i + shiftH, landscape.height_map[ii + 1][jj], j,
                    dHPlus, (ww % texture_density) / density_f,

                    i + shiftH, landscape.height_map[ii + 1][jj], j,
                    dHPlus, (ww % texture_density) / density_f,
                    i + shiftH, landscape.height_map[ii + 1][jj - 1], j - shiftW,
                    dHPlus, ((ww - 1) % texture_density) / density_f,
                    i, landscape.height_map[ii][jj], j,
                    (hh % texture_density) / density_f, (ww % texture_density) / density_f,
                    i, landscape.height_map[ii][jj - 1], j - shiftW,
                    (hh % texture_density) / density_f, ((ww - 1) % texture_density) / density_f,

                    i - shiftH, landscape.height_map[ii - 1][jj], j,
                    ((hh - 1) % texture_density) / density_f, (ww % texture_density) / density_f,
                    i, landscape.height_map[ii][jj], j,
                    (hh % texture_density) / density_f, (ww % texture_density) / density_f,
                    i - shiftH, landscape.height_map[ii - 1][jj - 1], j - shiftW,
                    ((hh - 1) % texture_density) / density_f, ((ww - 1) % texture_density) / density_f,
                    i, landscape.height_map[ii][jj - 1], j - shiftW,
                    (hh % texture_density) / density_f, ((ww - 1) % texture_density) / density_f,

                    i - shiftH, landscape.height_map[ii - 1][jj], j,
                    ((hh - 1) % texture_density) / density_f, (ww % texture_density) / density_f,
                    i - shiftH, landscape.height_map[ii - 1][jj + 1], j + shiftW,
                    ((hh - 1) % texture_density) / density_f, dWPlus,
                    i, landscape.height_map[ii][jj], j,
                    (hh % texture_density) / density_f, (ww % texture_density) / density_f,
                    i, landscape.height_map[ii][jj + 1], j + shiftW,
                    (hh % texture_density) / density_f, dWPlus
            };

            unsigned int currentIndices[] = {
                    0, 1, 2, 2, 1, 3
            };

            for (int k = 0; k < 5 * 4 * 4; k++) {
                vertices.push_back(current[k]);
            }

            for (int q = 0; q < 4; q++) {
                for (int k = 0; k < 6; k++) {
                    indices.push_back(indicesOffset + currentIndices[k]);
                }
                indicesOffset += 4;
            }
        }
    }

    unsigned int VBO, VAO, EBO;

    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), indices.data(), GL_STATIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void *) 0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void *) (sizeof(float) * 3));

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    landscape.mesh.IndexCount = indices.size();
    landscape.mesh.VAO = VAO;
    landscape.mesh.textures.push_back(load_tile_tex(texture1));
    landscape.mesh.textures.push_back(load_tile_tex(texture2));
    landscape.mesh.textures.push_back(load_tile_tex(texture3));

    landscape.threshold2 = threshold2;
    landscape.threshold1 = threshold1;

    landscape.scale = scale;
    landscape.width = width;
    landscape.height = height;
}


unsigned int create_frame_buffer() {
    unsigned int FBO;
    glGenFramebuffers(1, &FBO);
    glBindFramebuffer(GL_FRAMEBUFFER, FBO);
    return FBO;
}

unsigned int create_texture_attachment(int height, int width) {
    unsigned int textureID;
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_2D, textureID);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, nullptr);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, textureID, 0);
    return textureID;
}

unsigned int create_depth_texture_attachment(int height, int width) {
    unsigned int textureID;
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_2D, textureID);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, width, height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, nullptr);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, textureID, 0);

    glDrawBuffer(GL_NONE);
    glReadBuffer(GL_NONE);

    return textureID;
}

unsigned int create_depth_buffer_attachment(int height, int width) {
    unsigned int RBO;
    glGenRenderbuffers(1, &RBO);
    glBindRenderbuffer(GL_RENDERBUFFER, RBO);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, width, height);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, RBO);
    return RBO;
}

void
calculate_cascades(std::vector<glm::mat4> &lightProjections, std::vector<float> &cascadePlanes, glm::mat4 cameraView,
                   glm::mat4 lightView, int display_w, int display_h, float displayAngle) {
    float fovV = displayAngle;
    float ar = (float) display_w / (float) display_h;
    float fovH = glm::atan(glm::tan(fovV / 2) * ar) * 2;
    float tanV = glm::tan(fovV / 2);
    float tanH = glm::tan(fovH / 2);

    glm::mat4 cameraInverse = glm::inverse(cameraView);

    for (int i = 0; i < cascadePlanes.size() - 1; i++) {
        float xNear = cascadePlanes[i] * tanH;
        float xFar = cascadePlanes[i + 1] * tanH;
        float yNear = cascadePlanes[i] * tanV;
        float yFar = cascadePlanes[i + 1] * tanV;

        glm::vec4 frustum[8]{
                glm::vec4(xNear, yNear, -cascadePlanes[i], 1.0),
                glm::vec4(-xNear, yNear, -cascadePlanes[i], 1.0),
                glm::vec4(xNear, -yNear, -cascadePlanes[i], 1.0),
                glm::vec4(-xNear, -yNear, -cascadePlanes[i], 1.0),

                glm::vec4(xFar, yFar, -cascadePlanes[i + 1], 1.0),
                glm::vec4(-xFar, yFar, -cascadePlanes[i + 1], 1.0),
                glm::vec4(xFar, -yFar, -cascadePlanes[i + 1], 1.0),
                glm::vec4(-xFar, -yFar, -cascadePlanes[i + 1], 1.0)
        };

        glm::vec4 lightFrustum[8];

        float minX = 1000;
        float maxX = -1000;
        float minY = 1000;
        float maxY = -1000;
        float minZ = 1000;
        float maxZ = -1000;

        for (int j = 0; j < 8; j++) {
            glm::vec4 worldCorner = cameraInverse * frustum[j];
            lightFrustum[j] = lightView * worldCorner;
            minX = std::min(minX, lightFrustum[j].x);
            maxX = std::max(maxX, lightFrustum[j].x);
            minY = std::min(minY, lightFrustum[j].y);
            maxY = std::max(maxY, lightFrustum[j].y);
            minZ = std::min(minZ, lightFrustum[j].z);
            maxZ = std::max(maxZ, lightFrustum[j].z);
        }

        if (minZ > 0.0f)
            minZ = 0.0f;
        if (maxZ > 0.0f)
            maxZ = 0.0f;
        glm::mat4 lightProjection = glm::ortho(minX - 1, maxX + 1, minY - 1, maxY + 1, -maxZ - 1, -minZ + 1);
        lightProjections[i] = lightProjection;
    }
}

template<typename T>
int sgn(T val) {
    return (T(0) < val) - (val < T(0));
}

glm::mat4 calculate_oblique(glm::mat4 cameraProjection, glm::vec4 plane) {
    glm::mat4 inverse = glm::inverse(cameraProjection);
    glm::vec4 v = glm::vec4(sgn(plane.x), sgn(plane.y), 1.0f, 1.0f);
    glm::vec4 q = inverse * v;
    glm::vec4 c = plane * (2.0f / (glm::dot(plane, q)));

    glm::mat4 result = cameraProjection;
    result[2][0] = c.x;
    result[2][1] = c.y;
    result[2][2] = c.z + 1.0f;
    result[2][3] = c.w;

    return result;
}

#endif
