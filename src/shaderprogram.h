#ifndef SHADERPROGRAM_H
#define SHADERPROGRAM_H

#include <QOpenGLShaderProgram>
#include <QMatrix4x4>
#include <QVector3D>
#include <memory>
#include <chrono>

using Time = std::chrono::duration<float>;

class ShaderProgram {
public:
    enum AttribType {
        vertex,
        normal,
        position,
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
            std::string normalMatrixUniformName,
            std::string cameraPosUniformName);
    ShaderProgram(std::string vertexShaderFilePath,
            std::string fragmentShaderFilePath,
            std::string vertexAttribName,
            std::string normalAttribName,
            std::string positionAttribName,
            std::string mvpUniformName,
            std::string mMatrixUniformName,
            std::string normalMatrixUniformName,
            std::string cameraPosUniformName);
    ShaderProgram(std::string vertexShaderFilePath,
            std::string fragmentShaderFilePath,
            std::string vertexAttribName,
            std::string normalAttribName,
            std::string texCoordAttribName,
            std::string textureUniformName,
            std::string mvpUniformName,
            std::string mMatrixUniformName,
            std::string normalMatrixUniformName,
            std::string cameraPosUniformName);
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
            std::string normalMatrixUniformName,
            std::string cameraPosUniformName);
    void bind();
    void release();
    void bindTextureUnit(TextureType type, int textureUnit);
    void setMatrices(QMatrix4x4 vpMatrix, QMatrix4x4 mMatrix);
    void enableAttribs();
    void enablePosition();
    void disableAttribs();
    void setAttribFormat(AttribType attrType, GLenum eleType, int offset,
            int tupleSize, int stride);
    void setTime(Time time);
    void setCameraPos(QVector3D cameraPos);
    int getPositionLoc();
private:
    int getLocation(LocType type, std::string attribName);
    std::unique_ptr<QOpenGLShaderProgram> shaderProgram;
    int vertexAttrib;
    int normalAttrib;
    int positionAttrib;
    int texCoordAttrib;
    int textureUniform;
    int utilityTextureUniform;
    int timeUniform;
    int mvpUniform;
    int modelUniform;
    int normalMatrixUniform;
    int cameraPosUniform;
};

#endif // SHADER_H
