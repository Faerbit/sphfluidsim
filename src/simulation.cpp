#include "simulation.h"
#include "glfuncs.h"
#include "constants.h"
#include <cmath>
#include <algorithm>
#include <utility>
#include <sstream>
#include <iomanip>
#include <QApplication>
#include <ctime>
#include <cstdlib>

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

    sortedPositionsBuffer.create();
    sortedPositionsBuffer.bind();
    sortedPositionsBuffer.setUsagePattern(QOpenGLBuffer::DynamicDraw);
    sortedPositionsBuffer.release();

    sortedVelocitiesBuffer.create();
    sortedVelocitiesBuffer.bind();
    sortedVelocitiesBuffer.setUsagePattern(QOpenGLBuffer::DynamicDraw);
    sortedVelocitiesBuffer.release();

    voxelIndexBuffer.create();
    voxelIndexBuffer.bind();
    voxelIndexBuffer.setUsagePattern(QOpenGLBuffer::DynamicDraw);
    voxelIndexBuffer.release();

    neighbourBuffer.create();
    neighbourBuffer.bind();
    neighbourBuffer.setUsagePattern(QOpenGLBuffer::DynamicDraw);
    neighbourBuffer.release();

    densityPressureBuffer.create();
    densityPressureBuffer.bind();
    densityPressureBuffer.setUsagePattern(QOpenGLBuffer::DynamicDraw);
    densityPressureBuffer.release();

    dataBuffer.create();
    dataBuffer.bind();
    dataBuffer.setUsagePattern(QOpenGLBuffer::DynamicDraw);
    dataBuffer.allocate(2* sizeof(GLfloat));
    dataBuffer.release();

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

    vars.clear();
    vars.push_back(var_thread_size);
    sortPostPassShader = ComputeShader("sortPostPass.cmp", "work_items", vars);

    indexVoxelShader = ComputeShader("indexVoxel.cmp", "work_items", vars);    

    pair<string, string> var_max_no_interact_parts = 
        pair<string, string>("$MAX_NO_INTERACT_PARTICLES",
                str(MAX_NO_INTERACT_PARTICLES));
    pair<string, string> var_particle_mass = 
        pair<string, string>("$PARTICLE_MASS", str(PARTICLE_MASS));
    vars.push_back(var_domain_size_x);
    vars.push_back(var_domain_size_y);
    vars.push_back(var_domain_size_z);
    vars.push_back(var_interaction_radius);
    vars.push_back(var_max_no_interact_parts);
    findNeighboursShader = ComputeShader("findNeighbours.cmp",
            "work_items", vars);

    pair<string, string> var_resting_density =
        pair<string, string>("$RESTING_DENSITY", str(RESTING_DENSITY));
    pair<string, string> var_pressure_factor =
        pair<string, string>("$PRESSURE_FACTOR", str(PRESSURE_FACTOR));
    vars.clear();
    vars.push_back(var_thread_size);
    vars.push_back(var_interaction_radius);
    vars.push_back(var_resting_density);
    vars.push_back(var_pressure_factor);
    vars.push_back(var_particle_mass);
    vars.push_back(var_max_no_interact_parts);
    computeDensityPressureShader = ComputeShader("computeDensityPressure.cmp",
            "work_items", vars);

    pair<string, string> var_dynamic_viscosity =
        pair<string, string>("$DYNAMIC_VISCOSITY", str(DYNAMIC_VISCOSITY));
    pair<string, string> var_gravity = 
        pair<string, string>("$GRAVITY", str(GRAVITY));
    vars.clear();
    vars.push_back(var_thread_size);
    vars.push_back(var_interaction_radius);
    vars.push_back(var_resting_density);
    vars.push_back(var_particle_mass);
    vars.push_back(var_max_no_interact_parts);
    vars.push_back(var_dynamic_viscosity);
    vars.push_back(var_gravity);
    integrateShader = ComputeShader("integrate.cmp", "work_items", vars);

    srand(time(0));
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
    qDebug() << "Added particles: " << startPositions;
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

    sortedPositionsBuffer.bind();
    sortedPositionsBuffer.allocate(maxParticleCount * 4 * sizeof(GLfloat));
    sortedPositionsBuffer.release();

    sortedVelocitiesBuffer.bind();
    sortedVelocitiesBuffer.allocate(maxParticleCount * 4 * sizeof(GLfloat));
    sortedVelocitiesBuffer.release();

    neighbourBuffer.bind();
    neighbourBuffer.allocate(maxParticleCount * (MAX_NO_INTERACT_PARTICLES + 1)
            * sizeof(GLint));
    neighbourBuffer.release();

    densityPressureBuffer.bind();
    densityPressureBuffer.allocate(maxParticleCount * 2 * sizeof(GLfloat));
    densityPressureBuffer.release();

    int voxel_size_x = ceil(domain_size_x/INTERACTION_RADIUS);
    int voxel_size_y = ceil(domain_size_y/INTERACTION_RADIUS);
    int voxel_size_z = ceil(domain_size_z/INTERACTION_RADIUS);
    voxelCount = voxel_size_x * voxel_size_y * voxel_size_z;

    voxelIndexBuffer.bind();
    voxelIndexBuffer.allocate(voxelCount * 2 * sizeof(GLint));
    voxelIndexBuffer.release();
}

void Simulation::simulate(Time timeStep) {
    // voxelize particles
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
    voxelizeShader.release();
    positionsBuffer->release();
    partIndexBuffer.release();
    //debugPrintBuffer<GLfloat>("positions", positionsBuffer, 4);
    //debugPrintBuffer<GLint>("partIndex", partIndexBuffer, 2);

    // TODO sort on GPU
    vector<pair<GLint, GLint>> ind_buffer = vector<pair<GLint, GLint>>
        (maxParticleCount);
    partIndexBuffer.bind();
    partIndexBuffer.read(0, &ind_buffer[0], 2*maxParticleCount*sizeof(GLint));
    partIndexBuffer.release();
    sort(ind_buffer.begin(), ind_buffer.end(), 
            [](pair<GLint, GLint> a, pair<GLint, GLint> b) {
            return get<1>(a) < get<1>(b);
        });
    partIndexBuffer.bind();
    partIndexBuffer.write(0, &ind_buffer[0], 2*maxParticleCount*sizeof(GLint));
    partIndexBuffer.release();
    debugPrintBuffer<GLint>("partIndex", partIndexBuffer, 2, maxParticleCount);
    
    // sortPostPass
    partIndexBuffer.bind();
    glFuncs::funcs()->glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0,
            partIndexBuffer.bufferId());
    positionsBuffer->bind();
    glFuncs::funcs()->glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1,
            positionsBuffer->bufferId());
    velocitiesBuffer.bind();
    glFuncs::funcs()->glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 2,
            velocitiesBuffer.bufferId());
    sortedPositionsBuffer.bind();
    glFuncs::funcs()->glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 3,
            sortedPositionsBuffer.bufferId());
    sortedVelocitiesBuffer.bind();
    glFuncs::funcs()->glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 4,
            sortedVelocitiesBuffer.bufferId());
    sortPostPassShader.bind();
    sortPostPassShader.setWorkItems(maxParticleCount);
    glFuncs::funcs()->glDispatchCompute(invoke_count, 1, 1);
    sync();
    sortPostPassShader.release();
    partIndexBuffer.release();
    positionsBuffer->release();
    velocitiesBuffer.release();
    sortedPositionsBuffer.release();
    sortedVelocitiesBuffer.release();
    //debugPrintBuffer<GLfloat>("sortedPos", sortedPositionsBuffer, 4);
    //debugPrintBuffer<GLfloat>("sortedVel", sortedVelocitiesBuffer, 4);

    // index voxel
    voxelIndexBuffer.bind();
    glFuncs::funcs()->glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0,
            voxelIndexBuffer.bufferId());
    partIndexBuffer.bind();
    glFuncs::funcs()->glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1,
            partIndexBuffer.bufferId());
    indexVoxelShader.bind();
    indexVoxelShader.setWorkItems(voxelCount);
    invoke_count = ceil(float(voxelCount)/float(CMP_THREAD_SIZE));
    glFuncs::funcs()->glDispatchCompute(invoke_count, 1, 1);
    sync();
    indexVoxelShader.release();
    voxelIndexBuffer.release();
    partIndexBuffer.release();
    debugPrintBuffer<GLint>("voxelIndex", voxelIndexBuffer, 2, voxelCount);
    // find Neighbours
    partIndexBuffer.bind();
    glFuncs::funcs()->glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0,
            partIndexBuffer.bufferId());
    sortedPositionsBuffer.bind();
    glFuncs::funcs()->glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1,
            sortedPositionsBuffer.bufferId());
    voxelIndexBuffer.bind();
    glFuncs::funcs()->glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 2,
            voxelIndexBuffer.bufferId());
    neighbourBuffer.bind();
    glFuncs::funcs()->glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 3,
            neighbourBuffer.bufferId());
    dataBuffer.bind();
    updateData(timeStep);
    glFuncs::funcs()->glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 4,
            dataBuffer.bufferId());
    findNeighboursShader.bind();
    invoke_count = ceil(float(maxParticleCount)/float(CMP_THREAD_SIZE));
    findNeighboursShader.setWorkItems(maxParticleCount);
    glFuncs::funcs()->glDispatchCompute(invoke_count, 1, 1);
    sync();
    findNeighboursShader.release();
    partIndexBuffer.release();
    sortedPositionsBuffer.release();
    voxelIndexBuffer.release();
    neighbourBuffer.release();
    dataBuffer.release();
    debugPrintBuffer<GLint>("neighbours", neighbourBuffer,
            MAX_NO_INTERACT_PARTICLES + 1, maxParticleCount);
    // compute density and pressure
    neighbourBuffer.bind();
    glFuncs::funcs()->glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0,
            neighbourBuffer.bufferId());
    sortedPositionsBuffer.bind();
    glFuncs::funcs()->glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1,
            sortedPositionsBuffer.bufferId());
    densityPressureBuffer.bind();
    glFuncs::funcs()->glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 2,
            densityPressureBuffer.bufferId());
    computeDensityPressureShader.bind();
    computeDensityPressureShader.setWorkItems(maxParticleCount);
    glFuncs::funcs()->glDispatchCompute(invoke_count, 1, 1);
    sync();
    computeDensityPressureShader.release(); 
    neighbourBuffer.release();
    sortedPositionsBuffer.release();
    densityPressureBuffer.release();
    debugPrintBuffer<GLfloat>("densityPressure", densityPressureBuffer,
            2, maxParticleCount);
    // integrate
    neighbourBuffer.bind();
    glFuncs::funcs()->glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0,
            neighbourBuffer.bufferId());
    sortedPositionsBuffer.bind();
    glFuncs::funcs()->glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1,
            sortedPositionsBuffer.bufferId());
    sortedVelocitiesBuffer.bind();
    glFuncs::funcs()->glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 2,
            sortedVelocitiesBuffer.bufferId());
    densityPressureBuffer.bind();
    glFuncs::funcs()->glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 3,
            densityPressureBuffer.bufferId());
    positionsBuffer->bind();
    glFuncs::funcs()->glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 4,
            positionsBuffer->bufferId());
    velocitiesBuffer.bind();
    glFuncs::funcs()->glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 5,
            velocitiesBuffer.bufferId());
    dataBuffer.bind();
    glFuncs::funcs()->glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 6,
            dataBuffer.bufferId());
    invoke_count = ceil(float(maxParticleCount)/float(CMP_THREAD_SIZE));
    integrateShader.bind();
    integrateShader.setWorkItems(maxParticleCount);
    glFuncs::funcs()->glDispatchCompute(invoke_count, 1, 1);
    sync();
    integrateShader.release();
    neighbourBuffer.release();
    sortedPositionsBuffer.release();
    sortedVelocitiesBuffer.release();
    densityPressureBuffer.release();
    positionsBuffer->release();
    velocitiesBuffer.release();
    dataBuffer.release();
    debugPrintBuffer<GLfloat>("velocities", velocitiesBuffer,
            4, maxParticleCount);
    QApplication::quit();
}

// assumes dataBuffer is already bound
void Simulation::updateData(Time timeStep) {
    vector<GLfloat> data = vector<GLfloat>(2);
    data[0] = timeStep.count();
    // random float from 0.0 to 1.0
    data[1] = static_cast<GLfloat>(rand()) / static_cast<GLfloat>(RAND_MAX);
    dataBuffer.write(0, &data[0], 2 * sizeof(GLfloat));
}

template<typename T>
void Simulation::debugPrintBuffer(string name, shared_ptr<QOpenGLBuffer> buffer,
        int vec_size, int ele_count) {
    vector<T> debug_buffer = vector<T>(vec_size * ele_count);
    buffer->bind();
    if (buffer->read(0, &debug_buffer[0],
                vec_size * ele_count * sizeof(T))) {
        cout << "Buffer " << name << ": ";
        for (auto item : debug_buffer) {
            cout << item << ", ";
        }
        cout << endl;
    }
    else {
        cout << "Failed to read buffer" << endl;
    }
    buffer->release();
}

template<typename T>
void Simulation::debugPrintBuffer(string name, QOpenGLBuffer buffer,
        int vec_size, int ele_count) {
    vector<T> debug_buffer = vector<T>(vec_size * ele_count);
    buffer.bind();
    if (buffer.read(0, &debug_buffer[0],
                vec_size * ele_count * sizeof(T))) {
        cout << "Buffer " << name << ": ";
        for (auto item : debug_buffer) {
            //if(item != 1.55012e-41) {
            cout << item << ", ";
            //}
        }
        cout << endl;
    }
    else {
        cout << "Failed to read buffer" << endl;
    }
    buffer.release();
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

shared_ptr<QOpenGLBuffer> Simulation::getPositionsBuffer() {
    return positionsBuffer;
}

int Simulation::getParticleCount() {
    return maxParticleCount;
}
