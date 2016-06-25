#ifndef COMPUTESHADER_H
#define COMPUTESHADER_H

#include <memory>
#include <QOpenGLShaderProgram>

class ComputeShader {
public:
    ComputeShader();
    ComputeShader(std::string computeShaderFilePath,
            std::string timeUniformName);
    void bind();
    void release();
    void setTime(float time);
private:
    std::unique_ptr<QOpenGLShaderProgram> shaderProgram;
    int timeUniform;
};

#endif // COMPUTESHADER_H
