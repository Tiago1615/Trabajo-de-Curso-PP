#include "Trajectory.h"
#include <fstream>
#include <sstream>
#include <stdexcept>

vector<TrajectoryPoint> loadTrajectory(const string& path)
{
    vector<TrajectoryPoint> t;
    ifstream file(path);

    if (!file){
        throw runtime_error("No se pudo abrir trayectoria");
    }

    string line;
    while (getline(file, line))
    {
        if (line.empty() || line[0] == '#') continue;

        float time;
        TrajectoryPoint p;
        stringstream ss(line);
        ss >> time >> p.x >> p.y >> p.z >> p.theta;
        t.push_back(p);
    }

    return t;
}
