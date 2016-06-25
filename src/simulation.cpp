#include "simulation.h"

using namespace std;

Simulation::Simulation() {
}

Simulation::Simulation(float domain_size_x, float domain_size_y, float domain_size_z) {
    this->domain_size_x = domain_size_x;
    this->domain_size_y = domain_size_y;
    this->domain_size_z = domain_size_z;

    positionsBuffer = shared_ptr<QOpenGLBuffer>
        (new QOpenGLBuffer(QOpenGLBuffer::VertexBuffer));

    startPositions = vector<QVector3D>();
    startPositions.push_back(QVector3D());
    startPositions.push_back(QVector3D(0.0f, 2.0f, 0.0f));
    startPositions.push_back(QVector3D(0.0f, 4.0f, 0.0f));

    positionsBuffer->create();
    positionsBuffer->bind();
    // TODO use more appropritate usage pattern
    positionsBuffer->setUsagePattern(QOpenGLBuffer::StaticDraw);
    positionsBuffer->allocate(
            &startPositions[0], startPositions.size() * 3* sizeof(GLfloat));
    positionsBuffer->release();
}

shared_ptr<QOpenGLBuffer> Simulation::getPositionsBuffer() {
    return positionsBuffer;
}

int Simulation::getParticleCount() {
    return startPositions.size();
}
