#ifndef SIMULATION_H
#define SIMULATION_H

#include <memory>
#include <QOpenGLBuffer>
#include <QVector4D>
#include <chrono>

#include "computeshader.h"

using Clock = std::chrono::steady_clock;
using TimePoint = std::chrono::time_point<Clock>;
using Time = std::chrono::duration<float>;

class Simulation {
public:
    Simulation();
    Simulation(float domain_size_x, float domain_size_y, float domain_size_z);
    std::shared_ptr<QOpenGLBuffer> getPositionsBuffer();
    int getParticleCount();
    void simulate(Time time);
private:
    float domain_size_x = 0.0f;
    float domain_size_y = 0.0f;
    float domain_size_z = 0.0f;

    std::shared_ptr<QOpenGLBuffer> positionsBuffer;
    std::vector<QVector4D> startPositions;
    ComputeShader computeShader;
};

#endif // SIMULATION_H
