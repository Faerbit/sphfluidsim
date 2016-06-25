#include "computeshader.h"
#include "constants.h"
#include <iostream>

using namespace std;

ComputeShader::ComputeShader() {
    shaderProgram = nullptr;
}

ComputeShader::ComputeShader(std::string computeShaderFilePath,
        std::string timeUniformName) {
    shaderProgram = unique_ptr<QOpenGLShaderProgram>(new QOpenGLShaderProgram());
    if (!shaderProgram->addShaderFromSourceFile(QOpenGLShader::Compute,
            (SHADER_PATH + computeShaderFilePath).c_str())) {
        cerr << "Shader " << SHADER_PATH + computeShaderFilePath
            << " could not be loaded. Exiting."
            << endl;
        exit(1);
    }
    if(!shaderProgram->link()) {
        cerr << "Could not link shader " << SHADER_PATH + computeShaderFilePath
            << ". Log: " << endl;
        cerr << shaderProgram->log().toUtf8().constData();
    }
    timeUniform = shaderProgram->uniformLocation(timeUniformName.c_str());
    if (timeUniform < 0) {
        cerr << "Could not find uniform '" << timeUniformName << "'." << endl;
        exit(1);
    }
}

void ComputeShader::bind() {
    shaderProgram->bind();
}

void ComputeShader::release() {
    shaderProgram->release();
}

void ComputeShader::setTime(float time) {
    shaderProgram->setUniformValue(timeUniform, time);
}
