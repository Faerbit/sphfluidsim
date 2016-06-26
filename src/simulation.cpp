#include "simulation.h"
#include "glfuncs.h"
#include "constants.h"
#include <cmath>

using namespace std;

Simulation::Simulation() {
}

Simulation::Simulation(float domain_size_x, float domain_size_y, float domain_size_z) {
    this->domain_size_x = domain_size_x;
    this->domain_size_y = domain_size_y;
    this->domain_size_z = domain_size_z;

    positionsBuffer = shared_ptr<QOpenGLBuffer>
        (new QOpenGLBuffer(QOpenGLBuffer::VertexBuffer));

    startPositions = vector<QVector4D>();
    startPositions.push_back(QVector4D());
    startPositions.push_back(QVector4D(0.0f, 2.0f, 0.0f, 0.0f));
    startPositions.push_back(QVector4D(0.0f, 4.0f, 0.0f, 0.0f));

    positionsBuffer->create();
    positionsBuffer->bind();
    positionsBuffer->setUsagePattern(QOpenGLBuffer::DynamicDraw);
    positionsBuffer->allocate(
            &startPositions[0], startPositions.size() * 4 * sizeof(GLfloat));
    positionsBuffer->release();

    computeShader = ComputeShader("pos.cmp", "time", "work_items");
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
    glFuncs::funcs()->glDispatchCompute(invoke_count, 1, 1);
    glFuncs::funcs()->glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);
    computeShader.release();
    positionsBuffer->release();
}
