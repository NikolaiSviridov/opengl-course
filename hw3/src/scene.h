#pragma once
#ifndef HW3_SCENE_H
#define HW3_SCENE_H

#include <string>
#include <vector>
#include <map>
#include "opengl_shader.h"
#include "../includes/stb_image.h"
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtc/type_ptr.hpp>
#include<iostream>
#include "cubemap.h"
#include "land.h"
#include <util/mesh.h>
#include <util/model.h>
#include "camera.h"
#include "sun.h"
#include "projector.h"
#include "boat.h"
#include "lighthouse.h"

class Scene {
public:
    Camera camera;
    Lighthouse lighthouse;
    Boat boat;
    Sun sun;
    Land land;
    Projector projector;
    Cubemap cubemap;

    shader_t model_shader;
    shader_t simple_shader;
    shader_t land_shader;
    shader_t land_shader_shadow;
    shader_t model_shader_shadow;


    glm::mat4 projection;
    glm::mat4 model;
    glm::mat4 view;

    unsigned int cube;

    float water_normal;

    std::vector<unsigned int> shadow_depth_textures;
    std::vector<glm::mat4> light_space_matrices;
    std::vector<float> planes;

    void draw() {
        land_shader.use();
        model = glm::translate(model, glm::vec3(0, -0.05, 0));
        land_shader.set_uniform("model", glm::value_ptr(model));
        land_shader.set_uniform("view", glm::value_ptr(view));
        land_shader.set_uniform("projection", glm::value_ptr(projection));
        land_shader.set_uniform("lightSpaceMatrix1", glm::value_ptr(light_space_matrices[0]));
        land_shader.set_uniform("lightSpaceMatrix2", glm::value_ptr(light_space_matrices[1]));
        land_shader.set_uniform("lightSpaceMatrix3", glm::value_ptr(light_space_matrices[2]));
        land_shader.set_uniform("plane1", planes[1]);
        land_shader.set_uniform("plane2", planes[2]);
        land_shader.set_uniform("plane3", planes[3]);
        land_shader.set_uniform("sunPosition", sun.direction.x, sun.direction.y, sun.direction.z);
        land_shader.set_uniform("projectorPosition", projector.position.x, projector.position.y,
                                projector.position.z);
        land_shader.set_uniform("projectorDirection", projector.direction.x, projector.direction.y,
                                projector.direction.z);
        land_shader.set_uniform("projectorAngle", projector.angle);
        land_shader.set_uniform("cameraPosition", camera.position.x, camera.position.y, camera.position.z);
        land_shader.set_uniform("waterNormal", water_normal);

        glActiveTexture(GL_TEXTURE0 + 3);
        land_shader.set_uniform("shadowMap1", 3);
        glBindTexture(GL_TEXTURE_2D, shadow_depth_textures[0]);
        glActiveTexture(GL_TEXTURE0 + 4);
        land_shader.set_uniform("shadowMap2", 4);
        glBindTexture(GL_TEXTURE_2D, shadow_depth_textures[1]);
        glActiveTexture(GL_TEXTURE0 + 5);
        land_shader.set_uniform("shadowMap3", 5);
        glBindTexture(GL_TEXTURE_2D, shadow_depth_textures[2]);
        land.draw(land_shader);


        model = glm::translate(model, glm::vec3(0, 0.05, 0));
        model_shader.use();
        model = glm::translate(model, lighthouse.position);
        model_shader.set_uniform("model", glm::value_ptr(model));
        model_shader.set_uniform("view", glm::value_ptr(view));
        model_shader.set_uniform("projection", glm::value_ptr(projection));

        model_shader.set_uniform("sunPosition", sun.direction.x, sun.direction.y, sun.direction.z);
        model_shader.set_uniform("waterNormal", water_normal);
        model_shader.set_uniform("cameraPosition", camera.position.x, camera.position.y, camera.position.z);
        lighthouse.draw(model_shader);


        simple_shader.use();
        glBindVertexArray(cube);
        model = glm::translate(model, -lighthouse.position);
        model = glm::translate(model, projector.position);
        simple_shader.set_uniform("model", glm::value_ptr(model));
        simple_shader.set_uniform("view", glm::value_ptr(view));
        simple_shader.set_uniform("projection", glm::value_ptr(projection));
        simple_shader.set_uniform("waterNormal", water_normal);
        glDrawArrays(GL_TRIANGLES, 0, 36);
        glBindVertexArray(0);

        model = glm::translate(model, -projector.position);

        glm::mat4 vp = projection * glm::mat4(glm::mat3(view));
        cubemap.draw(vp);


        model = glm::translate(model, glm::vec3(0, -0.07, 0));
        model = glm::translate(model, boat.position);
        model = glm::rotate(model, 3.1415f, glm::vec3(0.0, 1.0, 0.0));
        model = glm::rotate(model, boat.boat_rotation + 3.1415f / 2, glm::vec3(0.0, 1.0, 0.0));

        model_shader.use();
        model_shader.set_uniform("model", glm::value_ptr(model));
        model_shader.set_uniform("view", glm::value_ptr(view));
        model_shader.set_uniform("projection", glm::value_ptr(projection));

        model_shader.set_uniform("sunPosition", sun.direction.x, sun.direction.y, sun.direction.z);
        model_shader.set_uniform("waterNormal", water_normal);
        model_shader.set_uniform("cameraPosition", camera.position.x, camera.position.y, camera.position.z);

        boat.draw(model_shader);
        model = glm::rotate(model, -3.1415f, glm::vec3(0.0, 1.0, 0.0));
        model = glm::rotate(model, -boat.boat_rotation - 3.1415f / 2, glm::vec3(0.0, 1.0, 0.0));
        model = glm::translate(model, glm::vec3(0, 0.07, 0));
        model = glm::translate(model, -boat.position);
    }

    void draw_shadow() {
        land_shader_shadow.use();
        model = glm::translate(model, glm::vec3(0, -0.05, 0));
        land_shader_shadow.set_uniform("model", glm::value_ptr(model));
        land_shader_shadow.set_uniform("view", glm::value_ptr(view));
        land_shader_shadow.set_uniform("projection", glm::value_ptr(projection));
        land.draw(land_shader_shadow);

        model = glm::translate(model, glm::vec3(0, 0.05, 0));

        model_shader_shadow.use();
        model = glm::translate(model, lighthouse.position);
        model_shader_shadow.set_uniform("model", glm::value_ptr(model));
        model_shader_shadow.set_uniform("view", glm::value_ptr(view));
        model_shader_shadow.set_uniform("projection", glm::value_ptr(projection));

        lighthouse.draw(model_shader);
        model = glm::translate(model, -lighthouse.position);

        simple_shader.use();
        glBindVertexArray(cube);
        model = glm::translate(model, projector.position);
        simple_shader.set_uniform("model", glm::value_ptr(model));
        simple_shader.set_uniform("view", glm::value_ptr(view));
        simple_shader.set_uniform("projection", glm::value_ptr(projection));
        glDrawArrays(GL_TRIANGLES, 0, 36);
        glBindVertexArray(0);

        model = glm::translate(model, -projector.position);

        model = glm::translate(model, glm::vec3(0, -0.07, 0));
        model = glm::translate(model, boat.position);
        model = glm::rotate(model, 3.1415f, glm::vec3(0.0, 1.0, 0.0));
        model = glm::rotate(model, boat.boat_rotation + 3.1415f / 2, glm::vec3(0.0, 1.0, 0.0));
        model_shader_shadow.use();
        model_shader_shadow.set_uniform("model", glm::value_ptr(model));
        model_shader_shadow.set_uniform("view", glm::value_ptr(view));
        model_shader_shadow.set_uniform("projection", glm::value_ptr(projection));

        boat.draw(model_shader_shadow);
        model = glm::rotate(model, -3.1415f, glm::vec3(0.0, 1.0, 0.0));
        model = glm::rotate(model, -boat.boat_rotation - 3.1415f / 2, glm::vec3(0.0, 1.0, 0.0));
        model = glm::translate(model, glm::vec3(0, 0.07, 0));
        model = glm::translate(model, -boat.position);
    }
};

#endif
