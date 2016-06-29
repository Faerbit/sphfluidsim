#include "computeshader.h"
#include "constants.h"
#include "glfuncs.h"
#include <iostream>
#include <fstream>
#include <streambuf>
#include <string>
#include <ctime>
#include <cstdlib>
#include <csignal>
#include "unistd.h"

using namespace std;

string ComputeShader::cur_shader_file_path = "";

ComputeShader::ComputeShader() {
    shaderProgram = nullptr;
}

string ComputeShader::loadShader(string fileName,
    std::vector<std::pair<std::string, std::string>> sourceVariables) {
    ifstream file(fileName);
    string content ((istreambuf_iterator<char>(file)),
                    istreambuf_iterator<char>());
    for (auto pair : sourceVariables) {
        string var = get<0>(pair);
        auto start_pos = content.find(var);
        if (start_pos == string::npos) {
            cout << "Failed to replace variable '" << var
                << "' in shader '" << fileName << "'." << endl;
            return content;
        }
        content.replace(start_pos, var.length(), get<1>(pair));
    }
    return content;
}

ComputeShader::ComputeShader(std::string computeShaderFilePath,
        std::string workItemsUniformName,
        std::vector<std::pair<std::string, std::string>> sourceVariables) {
    shaderProgram = unique_ptr<QOpenGLShaderProgram>(
            new QOpenGLShaderProgram());
    if (!shaderProgram->addShaderFromSourceCode(QOpenGLShader::Compute,
            loadShader(SHADER_PATH + computeShaderFilePath,
                sourceVariables).c_str())) {
        cerr << "Shader " << SHADER_PATH + computeShaderFilePath
            << " could not be loaded. Exiting."
            << endl;
        exit(1);
    }
    if(!shaderProgram->link()) {
        cerr << "Could not link shader " << SHADER_PATH + computeShaderFilePath
            << ". Log: " << endl;
        cerr << shaderProgram->log().toUtf8().constData();
        exit(1);
    }
    workItemsUniform = getLocation(workItemsUniformName);
    filePath = computeShaderFilePath;
    #ifdef WATCHDOG
        qDebug() << "Installing watchdog handler";
        signal(SIGALRM, ComputeShader::signal_handler);
    #endif 
}

int ComputeShader::getLocation(std::string uniformName) {
    int ret = shaderProgram->uniformLocation(uniformName.c_str());
    if (ret < 0) {
        cerr << "Could not find uniform '" << uniformName << "'." << endl;
        exit(1);
    }
    return ret;
}

void ComputeShader::signal_handler(int signal) {
    cerr << "Watchdog timer for shader '" << cur_shader_file_path
        << "' expired. Killing application." << endl;
    exit(1);
}

void ComputeShader::invoke(int invoke_count, int work_items) {
    bind();
    setWorkItems(work_items);
    #ifdef WATCHDOG
        cur_shader_file_path = filePath;
        // set watchdog
        alarm(1);
    #endif
    glFuncs::funcs()->glDispatchCompute(invoke_count, 1, 1);
    glFuncs::funcs()->glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);
    #ifdef WATCHDOG
        // deactivate watchdog
        alarm(0);
    #endif
    release();
}

void ComputeShader::bind() {
    shaderProgram->bind();
}

void ComputeShader::release() {
    shaderProgram->release();
}

void ComputeShader::setWorkItems(int items) {
    shaderProgram->setUniformValue(workItemsUniform, items);
}
