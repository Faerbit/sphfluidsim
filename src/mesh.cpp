#include "mesh.h"
#include "modelloader.h"
#include "constants.h"

using namespace std;

Mesh::Mesh () {
    ptr = nullptr;
}

Mesh::Mesh(string filePath) {
    ptr = MeshManager::load(filePath);
}

void Mesh::render(ShaderProgram* program, bool renderTextures) {
    ptr->vbo.bind();
    ptr->ibo.bind();

    program->enableAttribs();

    if (ptr->hasTexCoords) {
        if (renderTextures) {
            int offset = 0;
            int stride = 12* sizeof(GLfloat);
            program->setAttribFormat(ShaderProgram::vertex, GL_FLOAT, offset, 4,
                    stride);

            offset = 4 * sizeof(GLfloat);
            program->setAttribFormat(ShaderProgram::normal, GL_FLOAT, offset, 4,
                    stride);

            offset = 8 * sizeof(GLfloat);
            program->setAttribFormat(ShaderProgram::texCoord, GL_FLOAT, offset,
                    4, stride);
        }
        else {
            int offset = 0;
            int stride = 12* sizeof(GLfloat);
            program->setAttribFormat(ShaderProgram::vertex, GL_FLOAT, offset, 4,
                    stride);

            offset = 4 * sizeof(GLfloat);
            program->setAttribFormat(ShaderProgram::normal, GL_FLOAT, offset, 4,
                    stride);
        }
    }
    else {
        int offset = 0;
        int stride = 8* sizeof(GLfloat);
        program->setAttribFormat(ShaderProgram::vertex, GL_FLOAT, offset, 4,
                stride);

        offset = 4 * sizeof(GLfloat);
        program->setAttribFormat(ShaderProgram::normal, GL_FLOAT, offset, 4,
                stride);
    }

    glDrawElements(GL_TRIANGLES, ptr->indices.size(), GL_UNSIGNED_INT, 0);

    program->disableAttribs();

    ptr->ibo.release();
    ptr->vbo.release();
}

Mesh::sharedMesh Mesh::MeshManager::loadResource(string filePath) {
    sharedMesh mesh = sharedMesh(new MeshData());
    mesh->loadData(filePath);
    mesh->initVBO();
    mesh->initIBO();
    return mesh;
}

Mesh::MeshData::MeshData():
    vbo(QOpenGLBuffer::VertexBuffer), 
    ibo(QOpenGLBuffer::IndexBuffer) {
}

void Mesh::MeshData::loadData(string filePath) {
    ModelLoader model = ModelLoader();
    if (model.loadObjectFromFile(MESH_PATH + filePath)) {
        hasTexCoords = model.hasTextureCoordinates();
        vertices = vector<GLfloat>(model.lengthOfVBO(0, true, hasTexCoords));
        model.genVBO((GLfloat*) &vertices[0], 0, true, hasTexCoords);

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

void Mesh::MeshData::initVBO() {
    vbo.create();
    vbo.bind();
    vbo.setUsagePattern(QOpenGLBuffer::StaticDraw);
    vbo.allocate(&vertices[0], vertices.size() * sizeof(GLfloat));
    vbo.release();
}

void Mesh::MeshData::initIBO() {
    ibo.create();
    ibo.bind();
    ibo.setUsagePattern(QOpenGLBuffer::StaticDraw);
    ibo.allocate(&indices[0], indices.size() * sizeof(GLuint));
    ibo.release();
}
