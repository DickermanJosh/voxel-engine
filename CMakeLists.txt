cmake_minimum_required(VERSION 3.10)
project(minecraft)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Set the policy for FindOpenGL to prefer GLVND
cmake_policy(SET CMP0072 NEW)

# Find SFML
find_package(SFML 2.5 COMPONENTS graphics window system REQUIRED)

# Find OpenGL
find_package(OpenGL REQUIRED)

# Find GLEW
find_package(GLEW REQUIRED)

# Gind GLM
find_package(glm REQUIRED)

# Include Headers
include_directories(${CMAKE_SOURCE_DIR}/headers)
include_directories(${CMAKE_INCLUDE_DIRS})

# Add the executable
file(GLOB SOURCES "src/*.cpp", "src/utils/*.cpp")
add_executable(minecraft ${SOURCES})

# Link SFML and OpenGL libraries
target_link_libraries(minecraft sfml-graphics sfml-window sfml-system GLEW::GLEW ${OPENGL_LIBRARIES} glm::glm)

