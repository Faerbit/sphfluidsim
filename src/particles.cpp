#include "particles.h"
#include "modelloader.h"
#include "constants.h"
#include "glfuncs.h"

using namespace std;

Particles::Particles () {
    ptr = nullptr;
}

Particles::Particles(string particleModelfilePath,
        shared_ptr<QOpenGLBuffer> positionsBuffer,
        QVector3D basePosition,
        int particleCount, float partScale) {
    ptr = ParticlesManager::load(particleModelfilePath);
    this->basePosition = basePosition;
    scale = 1.0f;
    this->partScale= partScale * 0.5;
    this->positionsBuffer = positionsBuffer;
    this->particleCount = particleCount;
}

void Particles::render(ShaderProgram* program, QMatrix4x4 vpMatrix) {
    QMatrix4x4 modelMatrix;
    modelMatrix.translate(basePosition);
    modelMatrix.scale(scale);
    program->setMatrices(vpMatrix, modelMatrix);
    program->setScale(partScale);
    ptr->vbo.bind();
    ptr->ibo.bind();

    int offset = 0;
    int stride = 8* sizeof(GLfloat);
    program->setAttribFormat(ShaderProgram::vertex, GL_FLOAT, offset, 4,
            stride);

    offset = 4 * sizeof(GLfloat);
    program->setAttribFormat(ShaderProgram::normal, GL_FLOAT, offset, 4,
            stride);

    positionsBuffer->bind();
    offset = 0;
    stride = 4 * sizeof(GLfloat);
    program->setAttribFormat(ShaderProgram::position, GL_FLOAT, offset, 4,
            stride);
    program->enablePosition();
    glFuncs::funcs()->glVertexAttribDivisor(program->getPositionLoc(), 1);

    program->enableAttribs();

    glFuncs::funcs()->glDrawElementsInstanced(GL_TRIANGLES, ptr->indices.size(),
            GL_UNSIGNED_INT, 0, particleCount);

    program->disableAttribs();

    positionsBuffer->release();
    ptr->ibo.release();
    ptr->vbo.release();
}

Particles::sharedParticles Particles::ParticlesManager::loadResource(
        string filePath) {
    sharedParticles mesh = sharedParticles(new ParticlesData());
    mesh->loadData(filePath);
    mesh->initVBO();
    mesh->initIBO();
    return mesh;
}

Particles::ParticlesData::ParticlesData():
    vbo(QOpenGLBuffer::VertexBuffer),
    ibo(QOpenGLBuffer::IndexBuffer) {
}

void Particles::ParticlesData::loadData(string filePath) {
    ModelLoader model = ModelLoader();
    if (model.loadObjectFromFile(MESH_PATH + filePath)) {
        vertices = vector<GLfloat>(model.lengthOfVBO(0, true, false));
        model.genVBO((GLfloat*) &vertices[0], 0, true, false);

        indices = vector<GLuint>(model.lengthOfIndexArray());
        model.genIndexArray((GLuint*) &indices[0]);
    }
    else {
        cerr << "Model " << MESH_PATH + filePath
            << " could not be loaded. Exiting."
            << endl;
        exit(1);
    }
}

void Particles::ParticlesData::initVBO() {
    vbo.create();
    vbo.bind();
    vbo.setUsagePattern(QOpenGLBuffer::StaticDraw);
    vbo.allocate(&vertices[0], vertices.size() * sizeof(GLfloat));
    vbo.release();
}

void Particles::ParticlesData::initIBO() {
    ibo.create();
    ibo.bind();
    ibo.setUsagePattern(QOpenGLBuffer::StaticDraw);
    ibo.allocate(&indices[0], indices.size() * sizeof(GLuint));
    ibo.release();
}
