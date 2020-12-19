#pragma once
#ifndef HW3_LAND_H
#define HW3_LAND_H

#include <util/mesh.h>
#include <util/model.h>

struct Land {
    Mesh mesh;
    std::vector<std::vector<float>> height_map;
    float threshold1;
    float threshold2;
    int scale;
    int width;
    int height;

    void draw(shader_t &shader) {
        glActiveTexture(GL_TEXTURE0);
        shader.set_uniform("texture1", 0);
        glBindTexture(GL_TEXTURE_2D, mesh.textures[0].id);

        glActiveTexture(GL_TEXTURE0 + 1);
        shader.set_uniform("texture2", 1);
        glBindTexture(GL_TEXTURE_2D, mesh.textures[1].id);

        glActiveTexture(GL_TEXTURE0 + 2);
        shader.set_uniform("texture3", 2);
        glBindTexture(GL_TEXTURE_2D, mesh.textures[2].id);

        shader.set_uniform("threshold1", threshold1);
        shader.set_uniform("threshold2", threshold2);

        glBindVertexArray(mesh.VAO);
        glDrawElements(GL_TRIANGLES, mesh.IndexCount, GL_UNSIGNED_INT, 0);

        glBindVertexArray(0);
        glActiveTexture(GL_TEXTURE0);
    }

    float get_height(int x, int z) {
        return height_map[height - x * height / scale]
        [width - z * height / scale];
    }

};

#endif
