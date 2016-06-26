#ifndef COMPUTESHADER_H
#define COMPUTESHADER_H

#include <memory>
#include <QOpenGLShaderProgram>

class ComputeShader {
public:
    ComputeShader();
    ComputeShader(std::string computeShaderFilePath,
            std::string timeUniformName,
            std::string workItemsUniformName);
    void bind();
    void release();
    void setTime(float time);
    void setWorkItems(int items);
private:
    std::string loadShader(std::string fileName);
    std::unique_ptr<QOpenGLShaderProgram> shaderProgram;
    int getLocation(std::string uniformName);
    int timeUniform;
    int workItemsUniform;
};

#endif // COMPUTESHADER_H
