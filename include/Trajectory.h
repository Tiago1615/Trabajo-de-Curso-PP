#pragma once
#include <vector>
#include <string>

struct TrajectoryPoint
{
    float x, y, z;
    float theta;
};

std::vector<TrajectoryPoint> loadTrajectory(const std::string& path);
