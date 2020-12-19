#include <iostream>
#include <vector>

#include <GL/glew.h>

#include <GLFW/glfw3.h>

#include <glm/gtx/transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "opengl_shader.h"
#include <util/model.h>
#include "scene.h"
#include "loaders.h"

#define STB_IMAGE_IMPLEMENTATION

#include "../includes/stb_image.h"
#include <util/filesystem.h>


void framebuffer_size_callback(GLFWwindow *window, int width, int height) {
    glViewport(0, 0, width, height);
}

unsigned int reflection_frame_buffer;
unsigned int reflection_texture;
unsigned int reflection_depth_buffer;
unsigned int refraction_frame_buffer;
unsigned int refraction_texture;
unsigned int refraction_depth_texture;

std::vector<unsigned int> shadow_frame_buffers;
std::vector<unsigned int> shadow_depth_textures;

const unsigned int WIDTH = 800;
const unsigned int HEIGHT = 800;

std::vector<float> planes{
        0.1, 15.0, 30.0, 200.0
};

std::vector<std::pair<int, int>> resolutions{
        {2048, 2048},
        {1024, 1024},
        {256,  256}
};

int main(int, char **) {
    if (!glfwInit())
        return 1;

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow *window = glfwCreateWindow(WIDTH, HEIGHT, "HW3", nullptr, nullptr);
    if (window == nullptr)
        return 1;
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSwapInterval(1);

    if (glewInit() != GLEW_OK) {
        std::cerr << "Failed to initialize OpenGL loader!\n";
        return 1;
    }

    glEnable(GL_DEPTH_TEST);

    Scene scene;

    Lighthouse lighthouse(FileSystem::getPath("assets/lighthouse/lighthouse.obj"),
                          FileSystem::getPath("assets/lighthouse/"), 4);
    scene.lighthouse = lighthouse;

    Boat boat(FileSystem::getPath("assets/boat/gondol.obj"), FileSystem::getPath("assets/boat/"), 10);
    scene.boat = boat;

    std::vector<std::string> faces
            {
                    FileSystem::getPath("assets/textures/cubemap/right.jpg"),
                    FileSystem::getPath("assets/textures/cubemap/left.jpg"),
                    FileSystem::getPath("assets/textures/cubemap/top.jpg"),
                    FileSystem::getPath("assets/textures/cubemap/bottom.jpg"),
                    FileSystem::getPath("assets/textures/cubemap/front.jpg"),
                    FileSystem::getPath("assets/textures/cubemap/back.jpg")
            };

    shader_t cubemap_shader(FileSystem::getPath("assets/shaders/cubemap_shader.vs"),
                            FileSystem::getPath("assets/shaders/cubemap_shader.fs"));

    Cubemap cubemap(faces, cubemap_shader);
    scene.cubemap = cubemap;

    Mesh water;
    load_water(
            water, FileSystem::getPath("assets/textures/water/normalMap.png"),
            FileSystem::getPath("assets/textures/water/waterDUDV.png"), 100.0, 60.0);

    Land land;
    load_land(land, FileSystem::getPath("assets/textures/land/hm.png"),
              FileSystem::getPath("assets/textures/land/texture1.jpg"),
              FileSystem::getPath("assets/textures/land/texture2.jpg"),
              FileSystem::getPath("assets/textures/land/texture3.jpg"));
    scene.land = land;


    scene.model_shader = shader_t(FileSystem::getPath("assets/shaders/model_shader.vs"),
                                  FileSystem::getPath("assets/shaders/model_shader.fs"));
    scene.simple_shader = shader_t(FileSystem::getPath("assets/shaders/simple_shader.vs"),
                                   FileSystem::getPath("assets/shaders/simple_shader.fs"));
    scene.land_shader = shader_t(FileSystem::getPath("assets/shaders/land_shader.vs"),
                                 FileSystem::getPath("assets/shaders/land_shader.fs"));
    scene.land_shader_shadow = shader_t(FileSystem::getPath("assets/shaders/land_shader.vs"),
                                        FileSystem::getPath("assets/shaders/empty.fs"));
    scene.model_shader_shadow = shader_t(FileSystem::getPath("assets/shaders/model_shader.vs"),
                                         FileSystem::getPath("assets/shaders/empty.fs"));

    shader_t waterShader(FileSystem::getPath("assets/shaders/water_shader.vs"),
                         FileSystem::getPath("assets/shaders/water_shader.fs"));

    Projector projector;
    projector.position = scene.lighthouse.position + glm::vec3(0, 0.8, 0);
    scene.projector = projector;

    scene.cube = Cubemap::load_vertices(1 / 64.0f);

    reflection_frame_buffer = create_frame_buffer();
    reflection_texture = create_texture_attachment(720, 1280);
    reflection_depth_buffer = create_depth_buffer_attachment(720, 1280);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    refraction_frame_buffer = create_frame_buffer();
    refraction_texture = create_texture_attachment(720, 1280);
    refraction_depth_texture = create_depth_texture_attachment(720, 1280);


    for (int i = 0; i < 3; i++) {
        shadow_frame_buffers.push_back(create_frame_buffer());
        shadow_depth_textures.push_back(create_depth_texture_attachment(resolutions[i].second, resolutions[i].first));
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }

    scene.planes = planes;

    const float fov = glm::radians(45.0f);
    float wind_velocity = 0.001f;
    float wind_factor = 0.0f;

    while (!glfwWindowShouldClose(window)) {

        if (glfwGetKey(window, GLFW_KEY_R) == GLFW_PRESS) {
            scene.model_shader = shader_t(FileSystem::getPath("assets/shaders/model_shader.vs"),
                                          FileSystem::getPath("assets/shaders/model_shader.fs"));
            scene.simple_shader = shader_t(FileSystem::getPath("assets/shaders/simple_shader.vs"),
                                           FileSystem::getPath("assets/shaders/simple_shader.fs"));
            waterShader = shader_t(FileSystem::getPath("assets/shaders/water_shader.vs"),
                                   FileSystem::getPath("assets/shaders/water_shader.fs"));
            scene.land_shader = shader_t(FileSystem::getPath("assets/shaders/land_shader.vs"),
                                         FileSystem::getPath("assets/shaders/land_shader.fs"));
            scene.land_shader_shadow = shader_t(FileSystem::getPath("assets/shaders/land_shader.vs"),
                                                FileSystem::getPath("assets/shaders/empty.fs"));
            scene.model_shader_shadow = shader_t(FileSystem::getPath("assets/shaders/model_shader.vs"),
                                                 FileSystem::getPath("assets/shaders/empty.fs"));
        }

        scene.camera.keyboard_callback(window);

        std::vector<glm::mat4> light_projections;
        std::vector<glm::mat4> light_space_matrices;
        for (int i = 0; i < 3; i++) {
            light_space_matrices.emplace_back(0.0);
            light_projections.emplace_back(0.0);
        }

        scene.shadow_depth_textures = shadow_depth_textures;
        scene.light_space_matrices = light_space_matrices;

        int display_w, display_h;
        glfwGetFramebufferSize(window, &display_w, &display_h);

        scene.boat.move();

        glViewport(0, 0, display_w, display_h);


        glm::mat4 Projection = glm::perspective(fov, (float) display_w / (float) display_h, 0.1f, 200.0f);
        scene.projection = Projection;
        scene.model = glm::mat4(1.0f);
        scene.view = glm::lookAt(
                scene.camera.position,
                scene.camera.direction + scene.camera.position,
                glm::vec3(0, 1, 0)
        );

        scene.projector.move();

        wind_factor += wind_velocity;
        if (wind_factor > 1.0) {
            wind_factor = 0;
        }

        glEnable(GL_CLIP_DISTANCE0);

        glBindFramebuffer(GL_FRAMEBUFFER, reflection_frame_buffer);
        glViewport(0, 0, 1280, 720);

        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        float camera_to_water_distance = scene.camera.position.y;
        scene.camera.position.y -= 2 * camera_to_water_distance;
        scene.camera.direction.y *= -1;
        scene.view = glm::lookAt(
                scene.camera.position,
                scene.camera.direction + scene.camera.position,
                glm::vec3(0, 1, 0)
        );

        glm::mat4 old_projection = scene.projection;
        glm::vec4 water_plane = glm::vec4(0, 1, 0, 0);
        Projection = calculate_oblique(old_projection, scene.view * water_plane);

        scene.water_normal = 1.0f;

        scene.draw();
        scene.camera.position.y += 2 * camera_to_water_distance;
        scene.camera.direction.y *= -1;
        scene.view = glm::lookAt(
                scene.camera.position,
                scene.camera.direction + scene.camera.position,
                glm::vec3(0, 1, 0)
        );
        scene.projection = old_projection;

        glBindFramebuffer(GL_FRAMEBUFFER, refraction_frame_buffer);

        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        scene.water_normal = -1.0f;
        scene.draw();

        glBindFramebuffer(GL_FRAMEBUFFER, 0);

        glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glDisable(GL_CLIP_DISTANCE0);

        glm::mat4 lightView = glm::lookAt(glm::vec3(0, -1, 0) + 10.0f *
                                                                glm::vec3(scene.sun.direction.x, scene.sun.direction.y,
                                                                          scene.sun.direction.z),
                                          glm::vec3(0.0f, 0.0f, 0.0f),
                                          glm::vec3(0.0f, 1.0f, 0.0f));

        calculate_cascades(light_projections, planes, scene.view, lightView, display_w, display_h, fov);

        glm::mat4 old_view = scene.view;
        old_projection = scene.projection;

        for (int i = 0; i < 3; i++) {
            glViewport(0, 0, resolutions[i].first, resolutions[i].second);
            glBindFramebuffer(GL_FRAMEBUFFER, shadow_frame_buffers[i]);
            glClear(GL_DEPTH_BUFFER_BIT);

            scene.view = lightView;
            scene.projection = light_projections[i];
            light_space_matrices[i] = scene.projection * scene.view;

            scene.draw_shadow();

            glBindFramebuffer(GL_FRAMEBUFFER, 0);
        }

        glViewport(0, 0, display_w, display_h);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        scene.view = old_view;
        scene.projection = old_projection;

        scene.shadow_depth_textures = shadow_depth_textures;
        scene.light_space_matrices = light_space_matrices;

        scene.draw();

        waterShader.use();
        waterShader.set_uniform("model", glm::value_ptr(scene.model));
        waterShader.set_uniform("view", glm::value_ptr(scene.view));
        waterShader.set_uniform("projection", glm::value_ptr(scene.projection));

        waterShader.set_uniform("sunPosition", scene.sun.direction.x, scene.sun.direction.y, -scene.sun.direction.z);
        waterShader.set_uniform("projectorPosition", scene.projector.position.x, scene.projector.position.y,
                                scene.projector.position.z);
        waterShader.set_uniform("projectorDirection", scene.projector.direction.x, scene.projector.direction.y,
                                scene.projector.direction.z);
        waterShader.set_uniform("projectorAngle", projector.angle);
        waterShader.set_uniform("cameraPosition", scene.camera.position.x, scene.camera.position.y,
                                scene.camera.position.z);
        waterShader.set_uniform("windFactor", wind_factor);

        glActiveTexture(GL_TEXTURE0);
        waterShader.set_uniform("reflection_texture", 0);
        glBindTexture(GL_TEXTURE_2D, reflection_texture);
        glActiveTexture(GL_TEXTURE0 + 1);
        waterShader.set_uniform("refraction_texture", 1);
        glBindTexture(GL_TEXTURE_2D, scene.land.mesh.textures[0].id);
        glActiveTexture(GL_TEXTURE0 + 2);
        waterShader.set_uniform("water_normal", 2);
        glBindTexture(GL_TEXTURE_2D, water.textures[0].id);

        glActiveTexture(GL_TEXTURE0 + 3);
        waterShader.set_uniform("water_dudv", 3);
        glBindTexture(GL_TEXTURE_2D, water.textures[1].id);


        glBindVertexArray(water.VAO);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glDeleteFramebuffers(1, &reflection_frame_buffer);
    glDeleteTextures(1, &reflection_texture);
    glDeleteRenderbuffers(1, &reflection_depth_buffer);
    glDeleteFramebuffers(1, &refraction_frame_buffer);
    glDeleteTextures(1, &refraction_texture);
    glDeleteTextures(1, &refraction_depth_texture);

    glfwDestroyWindow(window);
    glfwTerminate();

    return 0;
}
