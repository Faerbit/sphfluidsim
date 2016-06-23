#ifndef MESH_H
#define MESH_H

#include "resourcemanager.h"
#include <QOpenGLBuffer>
#include "shaderprogram.h"


class Mesh {
public:
    Mesh();
    Mesh(std::string filePath);
    void render(ShaderProgram* program, bool renderTextures);

private:
    struct MeshData {
        MeshData();
        std::vector<GLfloat> vertices;
        std::vector<GLuint> indices;
        QOpenGLBuffer vbo;
        QOpenGLBuffer ibo;

        void loadData(std::string filePath);
        void initVBO();
        void initIBO();
        bool hasTexCoords;
    };

    typedef std::shared_ptr<MeshData> sharedMesh;
    sharedMesh ptr;   

    class MeshManager : public ResourceManager<MeshData, MeshManager> {
    friend ResourceManager<MeshData, MeshManager>;
    private:
        static sharedMesh loadResource(std::string filePath);
    };
};

#endif // MESH_H
