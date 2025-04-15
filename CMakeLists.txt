cmake_minimum_required(VERSION 3.10)
project(minecraft)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# DISABLE EVENTUALLY!!
set(CMAKE_BUILD_TYPE Debug)

# Set the policy for FindOpenGL to prefer GLVND
cmake_policy(SET CMP0072 NEW)

include(FetchContent)

# Find OpenGL
find_package(OpenGL REQUIRED)

# Find / install SFML
find_package(SFML 2.5 COMPONENTS graphics window system REQUIRED)
if (NOT SFML_FOUND)
    message(STATUS "Could not find SFML v2.5. Fetching it now...")

    FetchContent_Declare(
        sfml
        GIT_REPOSITORY https://github.com/SFML/SFML.git
        GIT_TAG 2.6.2
    )

    FetchContent_MakeAcailable(sfml)
endif()

message(STATUS "Using SFML v${SFML_VERSION}")

# Find / install GLEW
find_package(GLEW REQUIRED)
if (NOT GLEW_FOUND)
    message(STATUS "Could not find GLEW. Fetching it now...")

    FetchContent_Declare(
        glew
        GIT_REPOSITORY https://github.com/nigels-com/glew.git
        GIT_TAG 2.2.0
    )

    FetchContent_MakeAcailable(glew)
endif()

message(STATUS "Using GLEW v${GLEW_VERSION}")

# Find / install GLM
find_package(glm REQUIRED)
if (NOT glm_FOUND)
    message(STATUS "Could not find GLM. Fetching it now...")

    FetchContent_Declare(
        glm
        GIT_REPOSITORY https://github.com/g-truck/glm.git
        GIT_TAG 1.0.1
    )

    FetchContent_MakeAcailable(glm)
endif()

message(STATUS "Using GLM v${glm_VERSION}")

# Include Headers
# include_directories(${CMAKE_SOURCE_DIR}/headers)
include_directories(${CMAKE_SOURCE_DIR}/include)
include_directories(${CMAKE_INCLUDE_DIRS})

# Add the executable
file(GLOB SOURCES "src/*.cpp", "src/utils/*.cpp", "src/blocks/*.cpp")
add_executable(minecraft ${SOURCES})

# Link SFML, GLEW, GLM, and OpenGL libraries
target_link_libraries(
    minecraft 
    sfml-graphics sfml-window sfml-system 
    GLEW::GLEW 
    ${OPENGL_LIBRARIES} 
    glm::glm
)

