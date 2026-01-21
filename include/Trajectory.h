#pragma once
#include <vector>
#include <string>

using namespace std;

struct TrajectoryPoint
{
    float x, y, z;
    float theta;
};

vector<TrajectoryPoint> loadTrajectory(const string& path);
