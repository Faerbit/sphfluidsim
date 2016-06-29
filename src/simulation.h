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
    Simulation(int maxParticleCount,
            float domain_size_x, float domain_size_y, float domain_size_z);
    std::shared_ptr<QOpenGLBuffer> getPositionsBuffer();
    int getParticleCount();
    void simulate(Time timeStep);
    // TODO add time point and initial velocity
    void addFluidCuboid(float maxPartShare,
            float pos_x, float pos_y, float pos_z,
            float size_x, float size_y, float size_z);
    void addFluidCube(float maxPartShare,
            float pos_x, float pos_y, float pos_z,
            float size);
    void init();
    float getMinDensity();
private:
    float domain_size_x = 0.0f;
    float domain_size_y = 0.0f;
    float domain_size_z = 0.0f;

    float minDensity = std::numeric_limits<float>::max();

    int maxParticleCount = 0;
    int voxelCount = 0;

    std::shared_ptr<QOpenGLBuffer> positionsBuffer = nullptr;
    QOpenGLBuffer velocitiesBuffer;
    QOpenGLBuffer partIndexBuffer;
    QOpenGLBuffer sortedPositionsBuffer;
    QOpenGLBuffer sortedVelocitiesBuffer;
    QOpenGLBuffer voxelIndexBuffer;
    QOpenGLBuffer dataBuffer;
    std::vector<QVector4D> startPositions;
    std::vector<QVector4D> startVelocities;
    std::vector<std::pair<GLint, GLint>> partIndex;
    ComputeShader voxelizeShader;
    ComputeShader sortPostPassShader;
    ComputeShader indexVoxelShader;
    ComputeShader physicsShader;
    void sync();
    std::string str(int i);
    std::string str(float f);
    void updateData(Time timeStep);
    template<typename T>
    void debugPrintBuffer(std::string name, 
            std::shared_ptr<QOpenGLBuffer> buffer, int vec_size, int ele_count);
    template<typename T>
    void debugPrintBuffer(std::string name, QOpenGLBuffer buffer, 
            int vec_size, int ele_count);
};

#endif // SIMULATION_H
