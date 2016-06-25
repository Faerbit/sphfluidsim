#ifndef SIMULATION_H
#define SIMULATION_H

#include <memory>
#include <QOpenGLBuffer>
#include <QVector3D>

class Simulation {
public:
    Simulation();
    Simulation(float domain_size_x, float domain_size_y, float domain_size_z);
    std::shared_ptr<QOpenGLBuffer> getPositionsBuffer();
    int getParticleCount();
private:
    float domain_size_x = 0.0f;
    float domain_size_y = 0.0f;
    float domain_size_z = 0.0f;

    std::shared_ptr<QOpenGLBuffer> positionsBuffer;
    std::vector<QVector3D> startPositions;
};

#endif // SIMULATION_H
