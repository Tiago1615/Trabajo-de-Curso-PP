#pragma once
#include <vector>
#include <glad/glad.h>
#include <glm/glm.hpp>

class Renderer
{
    public:
        void initGrid();
        void initWalls(const std::vector<float>& wallVertices);
        void initTrajectory(const std::vector<float>& trajVertices);
        void initFloor(float size);
        void initAgent(float radius);

        void drawGrid() const;
        void drawWalls() const;
        void drawTrajectory() const;
        void drawFloor() const;
        void drawAgent(const glm::vec3& position, float theta) const;

        void addWall(std::vector<float>& v, float x0, float y0, float x1, float y1, float h);
    private:
        GLuint gridVAO = 0, gridVBO = 0;
        GLuint wallVAO = 0, wallVBO = 0;
        GLuint trajVAO = 0, trajVBO = 0;
        GLuint floorVAO = 0, floorVBO = 0;
        GLuint agentVAO = 0, agentVBO = 0;
        GLsizei gridCount = 0;
        GLsizei wallCount = 0;
        GLsizei trajCount = 0;
        GLsizei floorCount = 0;
        GLsizei agentVertexCount = 0;
};
