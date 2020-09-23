#include <iostream>

#include <fmt/format.h>

#include <GL/glew.h>

// Imgui + bindings
#include "imgui.h"
#include "bindings/imgui_impl_glfw.h"
#include "bindings/imgui_impl_opengl3.h"

// Include glfw3.h after our OpenGL definitions
#include <GLFW/glfw3.h>
#include "opengl_shader.h"

float vertexs[] = {
        -1.0f, -1.0f,
        1.0f, -1.0f,
        1.0f, 1.0f,
        -1.0f, 1.0f,
};
unsigned int indices[] = {0, 1, 2, 0, 2, 3};
float column_colors[] = {0 / 255.0f, 0 / 255.0f, 0 / 255.0f,
                         255 / 255.0f, 0 / 255.0f, 0 / 255.0f,
                         230 / 255.0f, 250 / 255.0f, 10 / 255.0f,
                         0 / 255.0f, 25 / 255.0f, 80 / 230.0f
};
static ImVec2 offset;
static float zoom = 1;
static int iterations = 50;
static float random_value[] = {0.f, 0.f};

static void glfw_error_callback(int error, const char *description) {
    std::cerr << fmt::format("Glfw Error {}: {}\n", error, description);
}

void init(GLuint &vbo, GLuint &vao, GLuint &ebo) {

    glGenVertexArrays(1, &vao);
    glGenBuffers(1, &vbo);
    glGenBuffers(1, &ebo);

    glBindVertexArray(vao);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);

    glBufferData(GL_ARRAY_BUFFER, sizeof(vertexs), vertexs, GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void *) 0);
    glEnableVertexAttribArray(0);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexImage1D(GL_TEXTURE_1D, 0, GL_RGB, 4, 0, GL_RGB, GL_FLOAT, column_colors);
    glBindTexture(GL_TEXTURE_1D, 0);

}


void scroll_callback(GLFWwindow *window, double xoffset, double yoffset) {
    ImGuiIO &io = ImGui::GetIO();
    ImVec2 size = io.DisplaySize;

    ImVec2 pos = ImGui::GetMousePos();
    pos.x = (pos.x / size.x * 2 - 1) * zoom;
    pos.y = (pos.y / size.y * 2 - 1) * zoom;

    zoom = yoffset > 0 ? zoom / 1.2f : zoom * 1.2f;

    ImVec2 new_pos = ImGui::GetMousePos();
    new_pos.x = (new_pos.x / size.x * 2 - 1) * zoom;
    new_pos.y = (new_pos.y / size.y * 2 - 1) * zoom;

    offset.x -= new_pos.x - pos.x;
    offset.y += new_pos.y - pos.y;
}

void mouse_drag(ImGuiIO &io) {
    if (!ImGui::IsAnyWindowFocused()) {
        ImVec2 display = io.DisplaySize;
        ImVec2 delta = ImGui::GetMouseDragDelta();
        ImGui::ResetMouseDragDelta();
        offset.x -= delta.x / display.x * zoom;
        offset.y += delta.y / display.y * zoom;
    }
}

int main(int, char **) {
    // Use GLFW to create a simple window
    glfwSetErrorCallback(glfw_error_callback);
    if (!glfwInit()) return 1;

    // GL 3.3 + GLSL 330
    const char *glsl_version = "#version 330";
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    //glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);            // 3.0+ only

    // Create window with graphics context
    GLFWwindow *window = glfwCreateWindow(1200, 800, "Fractal", NULL, NULL);
    if (window == NULL) return 1;
    glfwMakeContextCurrent(window);
    glfwSwapInterval(1); // Enable vsync

    // Initialize GLEW, i.e. fill all possible function pointers for current OpenGL context
    if (glewInit() != GLEW_OK) {
        std::cerr << "Failed to initialize OpenGL loader!\n";
        return 1;
    }

    // create our geometries
    GLuint vbo, vao, ebo;
    init(vbo, vao, ebo);

    // init shader
    shader_t fractal_shader("simple-shader.vs", "simple-shader.fs");

    // Setup GUI context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO &io = ImGui::GetIO();
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init(glsl_version);
    ImGui::StyleColorsDark();


    glfwSetScrollCallback(window, scroll_callback);

    while (!glfwWindowShouldClose(window)) {
        glfwPollEvents();

        // Get windows size
        int display_w, display_h;
        glfwGetFramebufferSize(window, &display_w, &display_h);

        // Set viewport to fill the whole window area
        glViewport(0, 0, display_w, display_h);

        // Fill background with solid color
        glClearColor(0.30f, 0.55f, 0.60f, 1.00f);
        glClear(GL_COLOR_BUFFER_BIT);

        mouse_drag(io);

        // Gui start new frame
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        // GUI
        ImGui::Begin("Settings");

        ImGui::Text("Quality settings");
        ImGui::SliderInt("Iterations", &iterations, 1, 100);

        ImGui::Spacing();

        ImGui::Text("Random constant for fun");
        ImGui::SliderFloat("X", &random_value[0], -1, 1);
        ImGui::SliderFloat("Y", &random_value[1], -1, 1);

        ImGui::Spacing();

        ImGui::Text("Color settings");
        ImGui::Columns(4, nullptr, false);
        for (int i = 0; i < 4; ++i) {
            ImGui::ColorEdit3(std::to_string(i).c_str(), column_colors + (3 * i), 1 << 5 | 1 << 7);
            ImGui::NextColumn();
        }

        ImGui::End();

        glActiveTexture(GL_TEXTURE0);
        glTexImage1D(GL_TEXTURE_1D, 0, GL_RGB, 4, 0, GL_RGB, GL_FLOAT, column_colors);

        // Pass the parameters to the shader as uniforms
        fractal_shader.set_uniform("u_iterations", iterations);
        fractal_shader.set_uniform("u_zoom", zoom);
        fractal_shader.set_uniform("u_offset", offset.x, offset.y);
        fractal_shader.set_uniform("u_random_value", random_value[0], random_value[1]);

        // Bind triangle shader
        fractal_shader.use();
        // Bind vertex array = buffers + indices
        glBindVertexArray(vao);

        // Execute draw call
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
        glBindVertexArray(0);

        // Generate gui render commands
        ImGui::Render();

        // Execute gui render commands using OpenGL backend
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        // Swap the backbuffer with the frontbuffer that is used for screen display
        glfwSwapBuffers(window);
    }

    // Cleanup
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    glfwDestroyWindow(window);
    glfwTerminate();

    return 0;
}


