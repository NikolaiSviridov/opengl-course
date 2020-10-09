cmake_minimum_required (VERSION 3.0)
cmake_policy(VERSION 3.0)

project (HW2)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/modules/")

link_directories(${CMAKE_SOURCE_DIR}/lib)

list(APPEND CMAKE_CXX_FLAGS "-std=c++11")

find_package(GLM REQUIRED)
message(STATUS "GLM included at ${GLM_INCLUDE_DIR}")
find_package(GLFW3 REQUIRED)
message(STATUS "Found GLFW3 in ${GLFW3_INCLUDE_DIR}")
find_package(ASSIMP REQUIRED)
message(STATUS "Found ASSIMP in ${ASSIMP_INCLUDE_DIR}")

set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -Wall")
find_package(OpenGL REQUIRED)
add_definitions(${OPENGL_DEFINITIONS})
find_package(X11 REQUIRED)

set(LIBS ${GLFW3_LIBRARY} X11 Xrandr Xinerama Xi Xxf86vm Xcursor GL dl pthread freetype ${ASSIMP_LIBRARY})
set (CMAKE_CXX_LINK_EXECUTABLE "${CMAKE_CXX_LINK_EXECUTABLE} -ldl")


configure_file(configuration/root_directory.h.in configuration/root_directory.h)
include_directories(${CMAKE_BINARY_DIR}/configuration)

add_library(STB_IMAGE "includes/stb_image.cpp")
set(LIBS ${LIBS} STB_IMAGE)

add_library(GLAD "includes/glad/glad.c")
set(LIBS ${LIBS} GLAD)

file(GLOB SOURCE
    "src/*.h"
    "src/*.cpp"
    "src/*.vs"
    "src/*.fs"
    "src/*.gs"
)
set(NAME "HW2_cubemap")
add_executable(${NAME} ${SOURCE})
target_link_libraries(${NAME} ${LIBS})
set_target_properties(${NAME} PROPERTIES RUNTIME_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/bin/")

# copy shader files to build directory
file(GLOB SHADERS
         "shaders/*.vs"
         "shaders/*.frag"
         "shaders/*.fs"
         "shaders/*.gs"
)

foreach(SHADER ${SHADERS})
        file(COPY ${SHADER} DESTINATION ${CMAKE_SOURCE_DIR}/bin/)
endforeach(SHADER)

include_directories(${CMAKE_SOURCE_DIR}/includes)
