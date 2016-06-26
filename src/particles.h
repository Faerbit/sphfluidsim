#ifndef PARTICLES_H
#define PARTICLES_H

#include "resourcemanager.h"
#include <QOpenGLBuffer>
#include "shaderprogram.h"
#include <QVector3D>
#include <QMatrix4x4>
#include <memory>


class Particles {
public:
    Particles();
    Particles(std::string filePath, 
            std::shared_ptr<QOpenGLBuffer> positionsBuffer, int particleCount);
    void render(ShaderProgram* program, QMatrix4x4 vpMatrix);

private:
    struct ParticlesData {
        ParticlesData();
        std::vector<GLfloat> vertices;
        std::vector<GLuint> indices;
        QOpenGLBuffer vbo;
        QOpenGLBuffer ibo;

        void loadData(std::string filePath);
        void initVBO();
        void initIBO();
    };

    typedef std::shared_ptr<ParticlesData> sharedParticles;
    sharedParticles ptr;
    int particleCount;
    std::shared_ptr<QOpenGLBuffer> positionsBuffer;
    QVector3D basePosition;
    float scale;
    float partScale;

    class ParticlesManager : public ResourceManager<ParticlesData, ParticlesManager> {
    friend ResourceManager<ParticlesData, ParticlesManager>;
    private:
        static sharedParticles loadResource(std::string filePath);
    };
};

#endif // PARTICLES_H
