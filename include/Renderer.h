#pragma once
#include <vector>
#include <glad/glad.h>

using namespace std;

class Renderer
{
    public:
        void initGrid();
        void initWalls(const vector<float>& wallVertices);
        void initTrajectory(const vector<float>& trajVertices);
        void initFloor(float size);

        void drawGrid() const;
        void drawWalls() const;
        void drawTrajectory() const;
        void drawFloor() const;

        void addWall(vector<float>& v, float x0, float y0, float x1, float y1, float h);
    private:
        GLuint gridVAO = 0, gridVBO = 0;
        GLuint wallVAO = 0, wallVBO = 0;
        GLuint trajVAO = 0, trajVBO = 0;
        GLuint floorVAO = 0, floorVBO = 0;
        GLsizei gridCount = 0;
        GLsizei wallCount = 0;
        GLsizei trajCount = 0;
        GLsizei floorCount = 0;
};
