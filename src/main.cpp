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
#include "Shaders_Utils.h"

using namespace std;

// ------------------------------------------------------------
// Utils
// ------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow*, int w, int h)
{
    glViewport(0, 0, w, h);
}

struct NamedColor {
    glm::vec3 rgb;
    string name;
};
const vector<NamedColor> palette = {
    {{0.90f, 0.20f, 0.20f}, "Red"},
    {{0.20f, 0.60f, 0.90f}, "Blue"},
    {{0.20f, 0.80f, 0.30f}, "Green"},
    {{0.90f, 0.80f, 0.20f}, "Yellow"},
    {{0.70f, 0.40f, 0.90f}, "Purple"},
    {{0.20f, 0.90f, 0.80f}, "Cyan"}
};

struct TrajectorySim
{
    vector<TrajectoryPoint> samples;
    GLuint VAO = 0;
    GLuint VBO = 0;
    GLsizei count = 0;

    glm::vec3 color;
    string colorName;
    string label;
    size_t currentIdx = 0;
};
vector<TrajectorySim> sims;

// ------------------------------------------------------------
// Main
// ------------------------------------------------------------
int main()
{
    // --------------------------------------------------------
    // Inicializar OpenGL (ventanas, contexto, glad)
    // --------------------------------------------------------
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 5);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* window = glfwCreateWindow(1280, 720, "Trabajo Curso PP - Trajectory Viewer", nullptr, nullptr);

    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);

    glViewport(0, 0, 1280, 720);
    glEnable(GL_DEPTH_TEST);

    cout << "OpenGL: " << glGetString(GL_VERSION) << endl << endl;

    cout << "=====================================\n";
    cout << "Trajectory Viewer - Controls\n";
    cout << "-------------------------------------\n";
    cout << "SPACE : Pause / Play\n";
    cout << "R     : Reset trajectory\n";
    cout << "=====================================\n";

    // --------------------------------------------------------
    // Shaders
    // --------------------------------------------------------
    GLuint program = createProgramFromFiles("../assets/shaders/basic.vert", "../assets/shaders/basic.frag");

    GLint uModelLoc = glGetUniformLocation(program, "uModel");
    GLint uColorLoc = glGetUniformLocation(program, "uColor");
    GLint uViewLoc  = glGetUniformLocation(program, "uView");
    GLint uProjLoc  = glGetUniformLocation(program, "uProj");
    GLint uLightDirLoc = glGetUniformLocation(program, "uLightDir");
    GLint uViewPosLoc  = glGetUniformLocation(program, "uViewPos");

    // --------------------------------------------------------
    // Cámara global
    // --------------------------------------------------------
    Camera camera;
    camera.setPosition({4.0f, -8.0f, 6.0f});
    camera.lookAt({3.0f, 3.0f, 1.0f});

    // --------------------------------------------------------
    // Renderer
    // --------------------------------------------------------
    Renderer renderer;
    renderer.initGrid();
    renderer.initFloor(10.0f);
    renderer.initAgent(0.25f);

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

    renderer.initWalls(walls);

    // --------------------------------------------------------
    // Cargar trayectorias
    // --------------------------------------------------------
    auto loadTrajectorySim = [&](const string& path, const string& label, int colorIdx)
    {
        TrajectorySim sim;
        sim.samples = loadTrajectory(path);
        sim.label = label;

        sim.color = palette[sims.size() % palette.size()].rgb;
        sim.colorName = palette[sims.size() % palette.size()].name;

        vector<float> verts;
        for (const auto& p : sim.samples){
            verts.push_back(p.x);
            verts.push_back(p.y);
            verts.push_back(p.z);
        }

        renderer.initTrajectory(sim.VAO, sim.VBO, sim.count, verts);
        sims.push_back(sim);
    };

    // Trayectorias donde se varía la velocidad lineal
    loadTrajectorySim("../assets/trajectories/vel/traj_v_0.50.txt", "v = 0.50 m/s", 0);
    loadTrajectorySim("../assets/trajectories/vel/traj_v_1.00.txt", "v = 1.00 m/s", 1);
    loadTrajectorySim("../assets/trajectories/vel/traj_v_1.50.txt", "v = 1.50 m/s", 2);

    // Trayectorias donde se varía la velocidad angular
    loadTrajectorySim("../assets/trajectories/vel_angular/traj_omega_0.79.txt", "w = 0.79 rad/s", 3);
    loadTrajectorySim("../assets/trajectories/vel_angular/traj_omega_1.57.txt", "w = 1.57 rad/s", 4);
    loadTrajectorySim("../assets/trajectories/vel_angular/traj_omega_3.14.txt", "w = 3.14 rad/s", 5);

    // --------------------------------------------------------
    // Parámetros de control de la simulación
    // --------------------------------------------------------

    float t = 0.f;
    float last = glfwGetTime();
    const float trajDt = 0.02f;

    bool paused = false;

    // para evitar múltiples activaciones por pulsación
    bool spacePressedLastFrame = false;
    bool rPressedLastFrame = false;

    // --------------------------------------------------------
    // Bucle de renderizado
    // --------------------------------------------------------

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

            for (auto& sim : sims)
            {
                size_t idx = (size_t)(t / trajDt);
                sim.currentIdx = std::min(idx, sim.samples.size() - 1);
            }
        }

        // --------------------------------------
        // Leyenda
        // --------------------------------------

        static bool printedLegend = false;
        if (!printedLegend){
            for (size_t i = 0; i < sims.size(); ++i){
                cout << "[" << i << "] "
                    << sims[i].label
                    << " | Color: " << sims[i].colorName << "\n";
            }
            cout << "=========================================\n";
            printedLegend = true;
        }

        glClearColor(0.1f, 0.1f, 0.12f, 1);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glUseProgram(program);

        glm::mat4 view = camera.getViewMatrix();
        glm::mat4 proj = glm::perspective(glm::radians(60.f), 1280.f / 720.f, 0.1f, 100.f);

        glUniformMatrix4fv(uViewLoc, 1, GL_FALSE, &view[0][0]);
        glUniformMatrix4fv(uProjLoc, 1, GL_FALSE, &proj[0][0]);

        glm::vec3 lightDir = glm::normalize(glm::vec3(-1.0f, -1.0f, -1.0f));
        glm::vec3 camPos = camera.getPosition();
        glUniform3fv(uLightDirLoc, 1, &lightDir[0]);
        glUniform3fv(uViewPosLoc, 1, &camPos[0]);

        glm::mat4 identity(1.0f);
        glUniformMatrix4fv(uModelLoc, 1, GL_FALSE, &identity[0][0]);

        // Suelo
        glEnable(GL_POLYGON_OFFSET_FILL);
        glPolygonOffset(1.0f, 1.0f);
        glUniform3f(uColorLoc, 0.25f, 0.25f, 0.25f);
        renderer.drawFloor();
        glDisable(GL_POLYGON_OFFSET_FILL);

        // Grid
        glUniform3f(uColorLoc,0.6f,0.6f,0.6f);
        renderer.drawGrid();

        // Trayectoria
        glUniformMatrix4fv(uModelLoc, 1, GL_FALSE, &identity[0][0]);
        glUniform3fv(uColorLoc, 1, &sims[0].color[0]);
        renderer.drawTrajectory(sims[0].VAO, sims[0].count);

        // Agentes
        for (auto& sim : sims){
            const auto& p = sim.samples[sim.currentIdx];

            glUniform3fv(uColorLoc, 1, &sim.color[0]);
            renderer.drawAgent({p.x, p.y, p.z}, p.theta);
        }

        // Paredes
        glUniformMatrix4fv(uModelLoc, 1, GL_FALSE, &identity[0][0]);
        glUniform3f(uColorLoc, 0.2f, 0.7f, 0.8f);
        renderer.drawWalls();

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}