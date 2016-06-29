#ifndef COMPUTESHADER_H
#define COMPUTESHADER_H

#include <memory>
#include <utility>
#include <QOpenGLShaderProgram>

class ComputeShader {
public:
    ComputeShader();
    ComputeShader(std::string computeShaderFilePath,
            std::string workItemsUniformName,
            std::vector<std::pair<std::string, std::string>> sourceVariables);
    void bind();
    void release();
    void invoke(int invoke_count, int work_items);
    void setWorkItems(int items);
private:
    std::string loadShader(std::string fileName,
            std::vector<std::pair<std::string, std::string>> sourceVariables);
    std::unique_ptr<QOpenGLShaderProgram> shaderProgram;
    int getLocation(std::string uniformName);
    int workItemsUniform;
    std::string filePath;

    static void signal_handler(int signal);
    static std::string cur_shader_file_path;
};

#endif // COMPUTESHADER_H
