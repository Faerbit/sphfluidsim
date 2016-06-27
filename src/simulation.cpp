#include "simulation.h"
#include "glfuncs.h"
#include "constants.h"
#include <cmath>
#include <algorithm>
#include <utility>
#include <sstream>
#include <iomanip>

using namespace std;

Simulation::Simulation() {
}

Simulation::Simulation(int maxParticleCount, 
        float domain_size_x, float domain_size_y, float domain_size_z) {
    this->maxParticleCount = maxParticleCount;
    this->domain_size_x = domain_size_x;
    this->domain_size_y = domain_size_y;
    this->domain_size_z = domain_size_z;

    minDensity = numeric_limits<float>::max();

    positionsBuffer = shared_ptr<QOpenGLBuffer>
        (new QOpenGLBuffer(QOpenGLBuffer::VertexBuffer));

    startPositions = vector<QVector4D>();

    startVelocities = vector<QVector4D>();

    partIndex = vector<pair<GLint, GLint>>();

    positionsBuffer->create();
    positionsBuffer->bind();
    positionsBuffer->setUsagePattern(QOpenGLBuffer::DynamicDraw);
    positionsBuffer->release();

    velocitiesBuffer.create();
    velocitiesBuffer.bind();
    velocitiesBuffer.setUsagePattern(QOpenGLBuffer::DynamicDraw);
    velocitiesBuffer.release();

    partIndexBuffer.create();
    partIndexBuffer.bind();
    partIndexBuffer.setUsagePattern(QOpenGLBuffer::DynamicDraw);
    partIndexBuffer.release();

    pair<string, string> var_thread_size = 
        pair<string, string>("$THREAD_SIZE", str(CMP_THREAD_SIZE));
    pair<string, string> var_domain_size_x = 
        pair<string, string>("$DOMAIN_SIZE_X", str(domain_size_x));
    pair<string, string> var_domain_size_y = 
        pair<string, string>("$DOMAIN_SIZE_Y", str(domain_size_y));
    pair<string, string> var_domain_size_z = 
        pair<string, string>("$DOMAIN_SIZE_Z", str(domain_size_z));
    pair<string, string> var_interaction_radius = 
        pair<string, string>("$INTERACTION_RADIUS",
                str(INTERACTION_RADIUS));
    vector<pair<string, string>> vars = vector<pair<string, string>>();
    vars.push_back(var_thread_size);
    vars.push_back(var_domain_size_x);
    vars.push_back(var_domain_size_y);
    vars.push_back(var_domain_size_z);
    vars.push_back(var_interaction_radius);
    voxelizeShader = ComputeShader("voxelize.cmp", "work_items", vars);
}

void Simulation::addFluidCuboid(float maxPartShare,
        float pos_x, float pos_y, float pos_z,
        float size_x, float size_y, float size_z) {
    int partCount = floor(maxParticleCount / maxPartShare);
    float volume = size_x * size_y * size_z;
    int count_x = floor(size_x/cbrt(volume/partCount));
    int count_y = floor(size_y/cbrt(volume/partCount));
    int count_z = floor(size_z/cbrt(volume/partCount));
    float density_x = size_x/count_x;
    minDensity = min(minDensity, density_x);
    float density_y = size_y/count_y;
    minDensity = min(minDensity, density_y);
    float density_z = size_z/count_z;
    minDensity = min(minDensity, density_z);
    qDebug() << "Adding " << count_x * count_y * count_z << " Particles. " << 
        "Leaving " <<
         (1.0f - (((float)count_x * count_y * count_z) /(float)partCount)) * 100.0f
        << " \% unused." 
        << " x: " << count_x * density_x << " density_x: " << density_x
        << " y: " << count_y * density_y << " density_y: " << density_y
        << " z: " << count_z * density_z << " density_z: " << density_z
        << endl;
    for(int i = 0; i<count_x; i++) {
        for(int j = 0; j<count_y; j++) {
            for(int k = 0; k<count_z; k++) {
                startPositions.push_back(QVector4D(
                    pos_x + i * density_x,
                    pos_y + j * density_y,
                    pos_z + k * density_z,
                    0.0f
                ));
            }
        }
    }
}

void Simulation::addFluidCube(float maxPartShare,
        float pos_x, float pos_y, float pos_z,
        float size) {
    int partCount = floor(maxParticleCount / maxPartShare);
    float volume = size * size * size;
    int count = floor(cbrt(partCount));
    float density = size/count;
    minDensity=min(minDensity, density);
    qDebug() << "Adding " << count * count * count << " Particles. " << 
        "Leaving " <<
         (1.0f - (((float)count * count * count) /(float)partCount)) * 100.0f
        << " \% unused." 
        << " x: " << count * density
        << " y: " << count * density
        << " z: " << count * density
        << endl;
    for(int i = 0; i<count; i++) {
        for(int j = 0; j<count; j++) {
            for(int k = 0; k<count; k++) {
                startPositions.push_back(QVector4D(
                    pos_x + i * density,
                    pos_y + j * density,
                    pos_z + k * density,
                    0.0f
                ));
            }
        }
    }
}

void Simulation::init() {
    maxParticleCount = startPositions.size();
    positionsBuffer->bind();
    positionsBuffer->allocate(
            &startPositions[0], maxParticleCount * 4 * sizeof(GLfloat));
    positionsBuffer->release();

    // init with no velocity
    startVelocities.assign(maxParticleCount, QVector4D());

    velocitiesBuffer.bind();
    velocitiesBuffer.allocate(&startVelocities[0],
            maxParticleCount * 4 * sizeof(GLfloat));
    velocitiesBuffer.release();

    partIndexBuffer.bind();
    partIndexBuffer.allocate(maxParticleCount * 2 * sizeof(GLint));
    partIndexBuffer.release();
}

shared_ptr<QOpenGLBuffer> Simulation::getPositionsBuffer() {
    return positionsBuffer;
}

int Simulation::getParticleCount() {
    return startPositions.size();
}

void Simulation::simulate(Time time) {
    float fTime = time.count();
    positionsBuffer->bind();
    partIndexBuffer.bind();
    glFuncs::funcs()->glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0,
            positionsBuffer->bufferId());
    glFuncs::funcs()->glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1,
            partIndexBuffer.bufferId());
    voxelizeShader.bind();
    voxelizeShader.setWorkItems(maxParticleCount);
    int invoke_count = ceil(float(maxParticleCount)/float(CMP_THREAD_SIZE));
    glFuncs::funcs()->glDispatchCompute(invoke_count, 1, 1);
    sync();
    // debug out
    vector<GLfloat> buffer = vector<GLfloat>(4*maxParticleCount);
    qDebug() << "maxPartCount: " << maxParticleCount;
    if (positionsBuffer->read(0, &buffer[0],   5*4 /* sizeof(GLfloat)*/)) {
        cout << "pos buffer: ";
        for(auto item: buffer) {
            cout << item << ", ";
        }
        cout << endl;
    }
    else {
        qDebug() << "Failed to read buffer.";
    }
    vector<GLint> ind_buffer = vector<GLint>(2*maxParticleCount);
    qDebug() << "maxPartCount: " << maxParticleCount;
    if (partIndexBuffer.read(0, &ind_buffer[0],   maxParticleCount * 2 * sizeof(GLint))) {
        cout << "ind buffer: ";
        for(auto item: ind_buffer) {
            cout << item << ", ";
        }
        cout << endl;
        exit(0);
    }
    else {
        qDebug() << "Failed to read buffer.";
    }
    voxelizeShader.release();
    positionsBuffer->release();
    partIndexBuffer.release();
}

void Simulation::sync() {
    glFuncs::funcs()->glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);
}

float Simulation::getMinDensity() {
    return minDensity;
}

string Simulation::str(int i) {
    ostringstream buff;
    buff << setprecision(16) << i;
    return buff.str();
}

string Simulation::str(float f) {
    ostringstream buff;
    buff << setprecision(16) << f;
    return buff.str();
}
