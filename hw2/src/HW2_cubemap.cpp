#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <fmt/format.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <util/filesystem.h>
#include <util/shader.h>
#include <util/camera.h>
#include <util/model.h>
#include <iostream>

#include <imgui.h>
#include "../bindings/imgui_impl_glfw.h"
#include "../bindings/imgui_impl_opengl3.h"

Camera camera(glm::vec3(0.0f, 0.0f, 3.0f));
const unsigned int WIDTH = 800;
const unsigned int HEIGHT = 600;

unsigned int loadCubemap(vector<std::string> faces);

ImGuiIO init_imgui(GLFWwindow *window);

void wheel_callback(float &zoom);

GLFWwindow *init_opengl() {
    glfwInit();

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);
    glfwWindowHint(GLFW_SAMPLES, 4);

    GLFWwindow *window = glfwCreateWindow(
            WIDTH,
            HEIGHT,
            "Task-2-3Dobject",
            nullptr,
            nullptr
    );

    if (window == nullptr) {
        glfwTerminate();
        throw std::runtime_error("Failed to create GLFW window");
    }

    glfwMakeContextCurrent(window);


    glewExperimental = GL_TRUE;
    if (glewInit() != GLEW_OK) {
        throw std::runtime_error("Failed to initialize GLEW");
    }

    return window;
}

void drag(glm::mat4& model, float zoom) {
    if (!ImGui::IsAnyWindowFocused()) {
        auto[dx, dy] = ImGui::GetMouseDragDelta();
        ImGui::ResetMouseDragDelta();

        float rotX = -1 * glm::radians(dy / 2.0);
        float rotY = -1 * glm::radians(dx / 2.0);

        auto xAxis = glm::vec3(1, 0, 0);
        model = glm::rotate(model, rotX, glm::vec3(xAxis.x, xAxis.y, xAxis.z));
        model = glm::rotate(glm::mat4(1), rotX, glm::vec3(xAxis.x, xAxis.y, xAxis.z)) *
                glm::scale(glm::mat4(1.0f), glm::vec3(zoom, zoom, zoom));
        auto yAxis = glm::vec3(0, 1, 0);
//        model = glm::rotate(model, rotY, glm::vec3(yAxis.x, yAxis.y, yAxis.z));

        model = glm::rotate(glm::mat4(1), rotY, glm::vec3(yAxis.x, yAxis.y, yAxis.z)) *
                          glm::scale(glm::mat4(1.0f), glm::vec3(zoom, zoom, zoom));
    }
}

int main() {
    GLFWwindow *window = init_opengl();
    ImGuiIO io = init_imgui(window);


    Shader shader(FileSystem::getPath("shaders/object.vs").c_str(),
                  FileSystem::getPath("shaders/object.fs").c_str());
    Shader skyboxShader(FileSystem::getPath("shaders/skybox.vs").c_str(),
                        FileSystem::getPath("shaders/skybox.fs").c_str());

    float skyboxVertices[] = {
            // positions
            -1.0f, 1.0f, -1.0f,
            -1.0f, -1.0f, -1.0f,
            1.0f, -1.0f, -1.0f,
            1.0f, -1.0f, -1.0f,
            1.0f, 1.0f, -1.0f,
            -1.0f, 1.0f, -1.0f,

            -1.0f, -1.0f, 1.0f,
            -1.0f, -1.0f, -1.0f,
            -1.0f, 1.0f, -1.0f,
            -1.0f, 1.0f, -1.0f,
            -1.0f, 1.0f, 1.0f,
            -1.0f, -1.0f, 1.0f,

            1.0f, -1.0f, -1.0f,
            1.0f, -1.0f, 1.0f,
            1.0f, 1.0f, 1.0f,
            1.0f, 1.0f, 1.0f,
            1.0f, 1.0f, -1.0f,
            1.0f, -1.0f, -1.0f,

            -1.0f, -1.0f, 1.0f,
            -1.0f, 1.0f, 1.0f,
            1.0f, 1.0f, 1.0f,
            1.0f, 1.0f, 1.0f,
            1.0f, -1.0f, 1.0f,
            -1.0f, -1.0f, 1.0f,

            -1.0f, 1.0f, -1.0f,
            1.0f, 1.0f, -1.0f,
            1.0f, 1.0f, 1.0f,
            1.0f, 1.0f, 1.0f,
            -1.0f, 1.0f, 1.0f,
            -1.0f, 1.0f, -1.0f,

            -1.0f, -1.0f, -1.0f,
            -1.0f, -1.0f, 1.0f,
            1.0f, -1.0f, -1.0f,
            1.0f, -1.0f, -1.0f,
            -1.0f, -1.0f, 1.0f,
            1.0f, -1.0f, 1.0f
    };

    glEnable(GL_DEPTH_TEST);

    unsigned int skyboxVAO, skyboxVBO;
    glGenVertexArrays(1, &skyboxVAO);
    glGenBuffers(1, &skyboxVBO);
    glBindVertexArray(skyboxVAO);
    glBindBuffer(GL_ARRAY_BUFFER, skyboxVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(skyboxVertices), &skyboxVertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void *) 0);

    Model object(FileSystem::getPath("resources/objects/Ucat.obj"));
    vector<std::string> faces
            {
                    FileSystem::getPath("resources/textures/skybox/right.jpg"),
                    FileSystem::getPath("resources/textures/skybox/left.jpg"),
                    FileSystem::getPath("resources/textures/skybox/top.jpg"),
                    FileSystem::getPath("resources/textures/skybox/bottom.jpg"),
                    FileSystem::getPath("resources/textures/skybox/front.jpg"),
                    FileSystem::getPath("resources/textures/skybox/back.jpg"),
            };
    unsigned int cubemapTexture = loadCubemap(faces);

    shader.use();
    shader.setInt("skybox", 0);

    skyboxShader.use();
    skyboxShader.setInt("skybox", 0);


    static float c_reflection;
    static float c_refraction;
    static float fresnel = 1.0;
    float zoom = 0.1f;

    while (!glfwWindowShouldClose(window)) {
        glfwPollEvents();

        // Settings
        int frame_width, frame_height;
        glfwGetFramebufferSize(window, &frame_width, &frame_height);
        glViewport(0, 0, frame_width, frame_height);

        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // ImGui
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();

        ImGui::NewFrame();
        ImGui::Begin("Settings");
        ImGui::SliderFloat("Refraction", &c_refraction, 0.0f, 1.0f);
        ImGui::SliderFloat("Reflection", &c_reflection, 0.0f, 1.0f);
        ImGui::SliderFloat("Fresnel", &fresnel, 0.0f, 2.0f);
        ImGui::End();

        wheel_callback(zoom);

        shader.use();
        auto model = glm::scale(glm::mat4(1.0f), glm::vec3(zoom, zoom, zoom));

        const float radius = 10.0f;
        float camX = sin(glfwGetTime()) * radius;
        float camZ = cos(glfwGetTime()) * radius;
        glm::mat4 view = glm::lookAt(glm::vec3(camX, 0.0, camZ), glm::vec3(0.0, 0.0, 0.0), glm::vec3(0.0, 1.0, 0.0));
//        glm::mat4 view = camera.GetViewMatrix();
        glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float) WIDTH / (float) HEIGHT, 0.1f,
                                                100.0f);
        shader.setMat4("model", model);
        shader.setMat4("view", view);
        shader.setMat4("projection", projection);
        shader.setVec3("cameraPos", camera.Position);
        shader.setFloat("c_reflection", c_reflection);
        shader.setFloat("c_refraction", c_refraction);
        shader.setFloat("fresnel", fresnel);

        glBindTexture(GL_TEXTURE_CUBE_MAP, cubemapTexture);
        object.Draw(skyboxShader);

        glDepthFunc(GL_LEQUAL);
        skyboxShader.use();
        view = glm::mat4(glm::mat3(camera.GetViewMatrix()));
        skyboxShader.setMat4("view", view);
        skyboxShader.setMat4("projection", projection);

        glBindVertexArray(skyboxVAO);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_CUBE_MAP, cubemapTexture);
        glDrawArrays(GL_TRIANGLES, 0, 36);
        glBindVertexArray(0);
        glDepthFunc(GL_LESS);

        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
        glfwSwapBuffers(window);
    }

    glDeleteVertexArrays(1, &skyboxVAO);
    glDeleteBuffers(1, &skyboxVAO);

    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
    glfwTerminate();
    return 0;
}

unsigned int loadCubemap(vector<std::string> faces) {
    unsigned int textureID;
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);

    int width, height, nrComponents;
    for (unsigned int i = 0; i < faces.size(); i++) {
        unsigned char *data = stbi_load(faces[i].c_str(), &width, &height, &nrComponents, 0);
        if (data) {
            glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE,
                         data);
            stbi_image_free(data);
        } else {
            std::cout << "Cubemap texture failed to load at path: " << faces[i] << std::endl;
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

ImGuiIO init_imgui(GLFWwindow *window) {
    const char *glsl_version = "#version 330";

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO &io = ImGui::GetIO();
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init(glsl_version);
    ImGui::StyleColorsDark();

    return io;
}

void wheel_callback(float &zoom) {
    auto wheel = ImGui::GetIO().MouseWheel;
    if (abs(wheel) > 0.1) {
        zoom *= wheel > 0 ? 1.1f : 1 / 1.1f;
    }
}