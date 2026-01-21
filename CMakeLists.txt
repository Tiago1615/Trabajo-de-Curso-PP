cmake_minimum_required(VERSION 3.16)
project(TrabajoCursoPP LANGUAGES C CXX)

# -----------------------------
# Configuración C++
# -----------------------------
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# -----------------------------
# Dependencias
# -----------------------------
find_package(OpenGL REQUIRED)
find_package(glfw3 REQUIRED)
find_package(glm REQUIRED)

# -----------------------------
# Ejecutable
# -----------------------------
add_executable(TrabajoCursoPP
    src/main.cpp
    src/Camera.cpp
    src/Trajectory.cpp
    src/Renderer.cpp
    dependencies/glad/src/glad.c
)

set_source_files_properties(
    dependencies/glad/src/glad.c
    PROPERTIES
    LANGUAGE C
)

target_include_directories(TrabajoCursoPP PRIVATE
    include
    dependencies/glad/include
)

# -----------------------------
# Linkado
# -----------------------------
target_link_libraries(TrabajoCursoPP
    OpenGL::GL
    glfw
    glm::glm
)