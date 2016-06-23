#ifndef SHADERPROGRAM_H
#define SHADERPROGRAM_H

#include <QOpenGLShaderProgram>
#include <QMatrix4x4>
#include <memory>
#include <chrono>

using Time = std::chrono::duration<float>;

class ShaderProgram {
public:
    enum AttribType {
        vertex,
        normal,
        texCoord
    };
    enum LocType {
        attrib,
        uniform
    };
    enum TextureType {
        standard,
        utility
    };
    ShaderProgram();
    ShaderProgram(std::string vertexShaderFilePath,
            std::string fragmentShaderFilePath,
            std::string vertexAttribName,
            std::string normalAttribName,
            std::string mvpUniformName,
            std::string mMatrixUniformName,
            std::string normalMatrixUniformName);
    ShaderProgram(std::string vertexShaderFilePath,
            std::string fragmentShaderFilePath,
            std::string vertexAttribName,
            std::string normalAttribName,
            std::string texCoordAttribName,
            std::string textureUniformName,
            std::string mvpUniformName,
            std::string mMatrixUniformName,
            std::string normalMatrixUniformName);
    ShaderProgram(std::string vertexShaderFilePath,
            std::string fragmentShaderFilePath,
            std::string vertexAttribName,
            std::string normalAttribName,
            std::string texCoordAttribName,
            std::string textureUniformName,
            std::string utilityTextureUniformName,
            std::string timeUniformName,
            std::string mvpUniformName,
            std::string mMatrixUniformName,
            std::string normalMatrixUniformName);
    void bind();
    void release();
    void bindTextureUnit(TextureType type, int textureUnit);
    void setMatrices(QMatrix4x4 vpMatrix, QMatrix4x4 mMatrix);
    void enableAttribs();
    void disableAttribs();
    void setAttribFormat(AttribType attrType, GLenum eleType, int offset,
            int tupleSize, int stride);
    void setTime(Time time);
private:
    int getLocation(LocType type, std::string attribName);
    std::unique_ptr<QOpenGLShaderProgram> shaderProgram;
    int vertexAttrib;
    int normalAttrib;
    int texCoordAttrib;
    int textureUniform;
    int utilityTextureUniform;
    int timeUniform;
    int mvpUniform;
    int modelUniform;
    int normalMatrixUniform;
};

#endif // SHADER_H
