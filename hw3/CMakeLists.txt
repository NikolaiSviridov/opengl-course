cmake_minimum_required(VERSION 3.0)
project (HW3 CXX)
set(NAME "HW3")

set(CMAKE_PREFIX_PATH ${CMAKE_BINARY_DIR})
set(CMAKE_MODULE_PATH ${CMAKE_BINARY_DIR})

# CONFIG option is important so that CMake doesnt search for modules into the default modules directory
find_package(glfw REQUIRED CONFIG)
find_package(glew REQUIRED CONFIG)
find_package(fmt REQUIRED CONFIG)
find_package(glm REQUIRED CONFIG)

set(CMAKE_CXX_STANDARD 17)
file(GLOB SOURCES
        src/*.cpp src/*.h
        includes/util/*.cpp includes/util/*.h
        assets/shaders/*.vs assets/shaders/*.fs
        )

add_executable( ${NAME} ${SOURCES} )
configure_file(configuration/root_directory.h.in configuration/root_directory.h)
include_directories(${CMAKE_BINARY_DIR}/configuration)
target_link_libraries(${NAME}
                        GLEW::glew_s 
                        glfw::glfw 
                        fmt::fmt 
                        glm::glm
                        )
include_directories(./includes)
