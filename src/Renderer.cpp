#include "Renderer.h"

using namespace std;

void Renderer::initGrid(){
    vector<float> grid;

    for (int i = -10; i <= 10; ++i){
        grid.insert(grid.end(), {
            -10.f, (float)i, 0.f,
             10.f, (float)i, 0.f,
            (float)i, -10.f, 0.f,
            (float)i,  10.f, 0.f
        });
    }

    gridCount = grid.size() / 3;

    glGenVertexArrays(1, &gridVAO);
    glGenBuffers(1, &gridVBO);
    glBindVertexArray(gridVAO);
    glBindBuffer(GL_ARRAY_BUFFER, gridVBO);
    glBufferData(GL_ARRAY_BUFFER, grid.size() * sizeof(float), grid.data(), GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
}

void Renderer::initWalls(const vector<float>& wallVertices){
    wallCount = wallVertices.size() / 3;

    glGenVertexArrays(1, &wallVAO);
    glGenBuffers(1, &wallVBO);
    glBindVertexArray(wallVAO);
    glBindBuffer(GL_ARRAY_BUFFER, wallVBO);
    glBufferData(GL_ARRAY_BUFFER, wallVertices.size() * sizeof(float), wallVertices.data(), GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
}

void Renderer::initTrajectory(const vector<float>& trajVertices){
    trajCount = trajVertices.size() / 3;

    glGenVertexArrays(1, &trajVAO);
    glGenBuffers(1, &trajVBO);

    glBindVertexArray(trajVAO);
    glBindBuffer(GL_ARRAY_BUFFER, trajVBO);
    glBufferData(GL_ARRAY_BUFFER, trajVertices.size() * sizeof(float), trajVertices.data(), GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
}

void Renderer::initFloor(float size){
    float s = size;

    vector<float> floor = {
        -s, -s, 0.0f,
        s, -s, 0.0f,
        s,  s, 0.0f,

        -s, -s, 0.0f,
        s,  s, 0.0f,
        -s,  s, 0.0f
    };

    floorCount = floor.size() / 3;

    glGenVertexArrays(1, &floorVAO);
    glGenBuffers(1, &floorVBO);
    glBindVertexArray(floorVAO);
    glBindBuffer(GL_ARRAY_BUFFER, floorVBO);
    glBufferData(
        GL_ARRAY_BUFFER,
        floor.size() * sizeof(float),
        floor.data(),
        GL_STATIC_DRAW
    );

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
}

void Renderer::drawGrid() const{
    glBindVertexArray(gridVAO);
    glDrawArrays(GL_LINES, 0, gridCount);
}

void Renderer::drawWalls() const{
    glBindVertexArray(wallVAO);
    glDrawArrays(GL_TRIANGLES, 0, wallCount);
}

void Renderer::drawTrajectory() const{
    glBindVertexArray(trajVAO);
    glDrawArrays(GL_LINE_STRIP, 0, trajCount);
}

void Renderer::drawFloor() const{
    glBindVertexArray(floorVAO);
    glDrawArrays(GL_TRIANGLES, 0, floorCount);
}

void Renderer::addWall(vector<float>& v, float x0, float y0, float x1, float y1, float h){
    v.insert(v.end(), {
        x0, y0, 0.0f,
        x1, y1, 0.0f,
        x1, y1, h
    });

    v.insert(v.end(), {
        x0, y0, 0.0f,
        x1, y1, h,
        x0, y0, h
    });
} 
