#ifndef COMPUTESHADER_H
#define COMPUTESHADER_H

#include <memory>
#include <QOpenGLShaderProgram>

class ComputeShader {
public:
    ComputeShader();
    ComputeShader(std::string computeShaderFilePath);
    void bind();
    void release();
private:
    std::unique_ptr<QOpenGLShaderProgram> shaderProgram;
};

#endif // COMPUTESHADER_H
