#include "computeshader.h"
#include "constants.h"
#include <iostream>

using namespace std;

ComputeShader::ComputeShader() {
    shaderProgram = nullptr;
}

ComputeShader::ComputeShader(std::string computeShaderFilePath) {
    shaderProgram = unique_ptr<QOpenGLShaderProgram>(new QOpenGLShaderProgram());
    if (!shaderProgram->addShaderFromSourceFile(QOpenGLShader::Compute,
            (SHADER_PATH + computeShaderFilePath).c_str())) {
        cerr << "Shader " << SHADER_PATH + computeShaderFilePath
            << " could not be loaded. Exiting."
            << endl;
        exit(1);
    }
}

void ComputeShader::bind() {
    shaderProgram->bind();
}

void ComputeShader::release() {
    shaderProgram->release();
}
