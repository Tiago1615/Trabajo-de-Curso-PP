#include <iostream>
#include <fstream>
#include <sstream>
#include <stdexcept>
#include <algorithm>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "Camera.h"
#include "Renderer.h"
#include "Trajectory.h"

using namespace std;

// ------------------------------------------------------------
// Utils
// ------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow*, int w, int h)
{
    glViewport(0, 0, w, h);
}

string loadTextFile(const string& path)
{
    ifstream file(path);
    if (!file)
        throw runtime_error("No se pudo abrir: " + path);

    stringstream ss;
    ss << file.rdbuf();
    return ss.str();
}

GLuint compileShader(GLenum type, const char* src)
{
    GLuint s = glCreateShader(type);
    glShaderSource(s, 1, &src, nullptr);
    glCompileShader(s);

    int ok;
    glGetShaderiv(s, GL_COMPILE_STATUS, &ok);
    if (!ok)
    {
        char log[512];
        glGetShaderInfoLog(s, 512, nullptr, log);
        cerr << log << endl;
    }
    return s;
}

// ------------------------------------------------------------
// MAIN
// ------------------------------------------------------------
int main()
{
    // --------------------------------------------------------
    // GLFW / OpenGL
    // --------------------------------------------------------
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 5);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* window = glfwCreateWindow(
        1280, 720,
        "Trabajo Curso PP - Trajectory Viewer",
        nullptr, nullptr
    );

    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);
    glEnable(GL_DEPTH_TEST);

    cout << "OpenGL: " << glGetString(GL_VERSION) << endl;

    // --------------------------------------------------------
    // Shaders
    // --------------------------------------------------------
    string vsrc = loadTextFile("../assets/shaders/basic.vert");
    string fsrc = loadTextFile("../assets/shaders/basic.frag");

    GLuint vs = compileShader(GL_VERTEX_SHADER, vsrc.c_str());
    GLuint fs = compileShader(GL_FRAGMENT_SHADER, fsrc.c_str());

    GLuint program = glCreateProgram();
    glAttachShader(program, vs);
    glAttachShader(program, fs);
    glLinkProgram(program);
    glDeleteShader(vs);
    glDeleteShader(fs);

    // --------------------------------------------------------
    // Cargar trayectoria
    // --------------------------------------------------------
    auto trajectory = loadTrajectory("../assets/trajectories/traj_base.txt");

    vector<float> trajVertices;
    for (const auto& p : trajectory){
        trajVertices.push_back(p.x);
        trajVertices.push_back(p.y);
        trajVertices.push_back(p.z);
    }

    // --------------------------------------------------------
    // Crear subsistemas
    // --------------------------------------------------------
    Camera camera;
    Renderer renderer;

    renderer.initGrid();

    // paredes EXACTAS como definiste
    vector<float> walls;
    float H = 3.0f;

    // Grupo exterior
    renderer.addWall(walls, -2, 0, -2, 6, H);
    renderer.addWall(walls, -2, 6,  7, 6, H);
    renderer.addWall(walls,  7, 6,  7, 0, H);

    // Grupo interior
    renderer.addWall(walls,  2, 0,  2, 2, H);
    renderer.addWall(walls,  2, 2,  4, 2, H);
    renderer.addWall(walls,  4, 2,  4, 0, H);

    renderer.initTrajectory(trajVertices);
    renderer.initWalls(walls);
    renderer.initFloor(10.0f);

    bool paused = false;

    // para evitar múltiples toggles por pulsación
    bool spacePressedLastFrame = false;
    bool rPressedLastFrame = false;

    // --------------------------------------------------------
    // Loop
    // --------------------------------------------------------
    float t = 0.f;
    float last = glfwGetTime();
    const float trajDt = 0.02f;

    while (!glfwWindowShouldClose(window)){
        // --------------------------------------
        // INPUT
        // --------------------------------------
        bool spaceNow = glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS;
        bool rNow     = glfwGetKey(window, GLFW_KEY_R) == GLFW_PRESS;

        // Toggle pausa
        if (spaceNow && !spacePressedLastFrame)
        {
            paused = !paused;
        }
        spacePressedLastFrame = spaceNow;

        // Reset
        if (rNow && !rPressedLastFrame)
        {
            t = 0.0f;
        }
        rPressedLastFrame = rNow;

        float now = glfwGetTime();
        float dt = now - last;
        last = now;
        if (!paused)
        {
            t += dt;
        }

        int idx = min((int)(t / trajDt), (int)trajectory.size() - 1);
        const auto& p = trajectory[idx];

        camera.updateFromTrajectory(p.x, p.y, p.z, p.theta);

        glm::mat4 view = camera.getViewMatrix();
        glm::mat4 proj = glm::perspective(
            glm::radians(60.f),
            1280.f / 720.f,
            0.1f,
            100.f
        );

        glClearColor(0.1f, 0.1f, 0.12f, 1);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glUseProgram(program);

        glm::vec3 lightDir = glm::normalize(glm::vec3(-1.0f, -1.0f, -1.0f));
        glm::vec3 camPos = camera.getPosition();
        glUniform3fv(glGetUniformLocation(program, "uLightDir"), 1, &lightDir[0]);
        glUniform3fv(glGetUniformLocation(program, "uViewPos"), 1, &camPos[0]);

        glUniformMatrix4fv(glGetUniformLocation(program,"uView"),1,GL_FALSE,&view[0][0]);
        glUniformMatrix4fv(glGetUniformLocation(program,"uProj"),1,GL_FALSE,&proj[0][0]);

        // Floor
        glEnable(GL_POLYGON_OFFSET_FILL);
        glPolygonOffset(1.0f, 1.0f);
        glUniform3f(glGetUniformLocation(program, "uColor"), 0.25f, 0.25f, 0.25f);
        renderer.drawFloor();
        glDisable(GL_POLYGON_OFFSET_FILL);

        // Grid
        glUniform3f(glGetUniformLocation(program,"uColor"),0.6f,0.6f,0.6f);
        renderer.drawGrid();

        // Trajectory
        glUniform3f(glGetUniformLocation(program,"uColor"),0.9f,0.2f,0.2f);
        renderer.drawTrajectory();

        // Walls
        glUniform3f(glGetUniformLocation(program,"uColor"),0.2f,0.7f,0.8f);
        renderer.drawWalls();

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}