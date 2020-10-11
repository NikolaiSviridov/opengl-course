cmake_minimum_required(VERSION 3.0)
project (HW2 CXX)
set(NAME "HW2")

set(CMAKE_PREFIX_PATH ${CMAKE_BINARY_DIR})
set(CMAKE_MODULE_PATH ${CMAKE_BINARY_DIR})

# CONFIG option is important so that CMake doesnt search for modules into the default modules directory
find_package(imgui CONFIG)
find_package(glfw CONFIG)
find_package(glew CONFIG)
find_package(fmt CONFIG)
find_package(glm CONFIG)
#find_package(stb CONFIG)
find_package(glad CONFIG)
find_package(assimp CONFIG)
find_package(tinyobjloader CONFIG)

set(CMAKE_CXX_STANDARD 17)

add_library(STB_IMAGE "includes/stb_image.cpp")
file(GLOB SOURCES
        src/*.cpp src/*.h
        includes/util/*.cpp includes/util/*.h
        shaders/*.vs shaders/*.fs
        bindings/*.cpp bindings/*.hpp)

add_executable(${NAME} ${SOURCES})
configure_file(configuration/root_directory.h.in configuration/root_directory.h)
include_directories(${CMAKE_BINARY_DIR}/configuration)

#add_custom_command(
#        TARGET ${NAME}
#        POST_BUILD
#    #    COMMAND ${CMAKE_COMMAND} -E copy_directory ${PROJECT_SOURCE_DIR}/assets ${PROJECT_BINARY_DIR}/assets
#        COMMAND ${CMAKE_COMMAND} -E copy_directory ${PROJECT_SOURCE_DIR}/shaders ${PROJECT_BINARY_DIR}/shaders
#)

target_compile_definitions(${NAME} PUBLIC IMGUI_IMPL_OPENGL_LOADER_GLEW)
target_link_libraries(${NAME}
                        tinyobjloader::tinyobjloader
                        imgui::imgui
                        GLEW::glew_s
                        glfw::glfw
                        fmt::fmt
                        glm::glm
                #        stb::stb
                        glad::glad
                        assimp::assimp
                        STB_IMAGE
                        )

include_directories(./includes)