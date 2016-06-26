#include "simulation.h"
#include "glfuncs.h"
#include "constants.h"
#include <cmath>
#include <algorithm>

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

    startPositions = vector<QVector4D>(maxParticleCount);

    positionsBuffer->create();
    positionsBuffer->bind();
    positionsBuffer->setUsagePattern(QOpenGLBuffer::DynamicDraw);
    positionsBuffer->release();

    computeShader = ComputeShader("pos.cmp", "time", "work_items");
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
    cout << "Adding " << count_x * count_y * count_z << " Particles. " << 
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
    cout << "Adding " << count * count * count << " Particles. " << 
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
    positionsBuffer->bind();
    positionsBuffer->allocate(
            &startPositions[0], startPositions.size() * 4 * sizeof(GLfloat));
    positionsBuffer->release();
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
    /*vector<GLfloat> buffer = vector<GLfloat>(12);
    positionsBuffer->read(0, &buffer[0], 12*sizeof(GLfloat));
    cout << "buffer: ";
    for(auto item: buffer) {
        cout << item << ", ";
    }
    cout << endl;*/
    glFuncs::funcs()->glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0,
            positionsBuffer->bufferId());
    computeShader.bind();
    computeShader.setTime(fTime);
    computeShader.setWorkItems(startPositions.size());
    int invoke_count = ceil(float(startPositions.size())/float(CMP_THREAD_SIZE));
    //glFuncs::funcs()->glDispatchCompute(invoke_count, 1, 1);
    glFuncs::funcs()->glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);
    computeShader.release();
    positionsBuffer->release();
}

float Simulation::getMinDensity() {
    return minDensity;
}
