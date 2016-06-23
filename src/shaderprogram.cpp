#include "shaderprogram.h"
#include "constants.h"
#include <iostream>

using namespace std;

ShaderProgram::ShaderProgram() {
    shaderProgram = nullptr;
}

ShaderProgram::ShaderProgram(std::string vertexShaderFilePath,
        std::string fragmentShaderFilePath,
        std::string vertexAttribName,
        std::string normalAttribName,
        std::string mvpUniformName,
        std::string mMatrixUniformName,
        std::string normalMatrixUniformName,
        std::string cameraPosUniformName) {
    shaderProgram = unique_ptr<QOpenGLShaderProgram>(new QOpenGLShaderProgram());
    if (!shaderProgram->addShaderFromSourceFile(QOpenGLShader::Vertex,
            (SHADER_PATH + vertexShaderFilePath).c_str())) {
        cerr << "Shader " << SHADER_PATH + vertexShaderFilePath
            << " could not be loaded. Exiting."
            << endl;
        exit(1);
    }
    if (!shaderProgram->addShaderFromSourceFile(QOpenGLShader::Fragment,
            (SHADER_PATH + fragmentShaderFilePath).c_str())) {
        cerr << "Shader " << SHADER_PATH + fragmentShaderFilePath
            << " could not be loaded. Exiting."
            << endl;
        exit(1);
    }
    if(!shaderProgram->link()) {
        cerr << "Could not link shader " << SHADER_PATH + vertexShaderFilePath
            << ". Log: " << endl;
        cerr << shaderProgram->log().toUtf8().constData();
    }
    vertexAttrib = getLocation(attrib, vertexAttribName);
    normalAttrib = getLocation(attrib, normalAttribName);
    texCoordAttrib = -1;
    textureUniform = -1;
    utilityTextureUniform = -1;
    timeUniform = -1;
    mvpUniform = getLocation(uniform, mvpUniformName);
    modelUniform = getLocation(uniform, mMatrixUniformName);
    normalMatrixUniform = getLocation(uniform, normalMatrixUniformName);
    cameraPosUniform = getLocation(uniform, cameraPosUniformName);
}

ShaderProgram::ShaderProgram(std::string vertexShaderFilePath,
            std::string fragmentShaderFilePath,
            std::string vertexAttribName,
            std::string normalAttribName,
            std::string texCoordAttribName,
            std::string textureUniformName,
            std::string mvpUniformName,
            std::string mMatrixUniformName,
            std::string normalMatrixUniformName,
            std::string cameraPosUniformName) :
    ShaderProgram(vertexShaderFilePath, fragmentShaderFilePath,
            vertexAttribName, normalAttribName, mvpUniformName,
            mMatrixUniformName, normalMatrixUniformName, cameraPosUniformName) {
        texCoordAttrib = getLocation(attrib, texCoordAttribName);
        textureUniform = getLocation(uniform, textureUniformName);
}

ShaderProgram::ShaderProgram(std::string vertexShaderFilePath,
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
            std::string cameraPosUniformName) :
    ShaderProgram(vertexShaderFilePath, fragmentShaderFilePath,
            vertexAttribName, normalAttribName, texCoordAttribName,
            textureUniformName, mvpUniformName, mMatrixUniformName,
            normalMatrixUniformName, cameraPosUniformName) {
    utilityTextureUniform = getLocation(uniform, utilityTextureUniformName);
    timeUniform = getLocation(uniform, timeUniformName);
}

int ShaderProgram::getLocation(LocType type, string attribName) {
    int ret = -1;
    if (type == attrib) {
        ret = shaderProgram->attributeLocation(attribName.c_str());
        if (ret < 0) {
            cerr << "Could not find attribute '" << attribName << "'." << endl;
        }
        return ret;
    }
    else if (type == uniform) {
        ret = shaderProgram->uniformLocation(attribName.c_str());
        if (ret < 0) {
            cerr << "Could not find uniform '" << attribName << "'." << endl;
        }
        return ret;
    }
    else {
        cerr << "Undefined location type for location '" << attribName << "'."
            << endl;
        exit(1);
    }
}

void ShaderProgram::bindTextureUnit(TextureType type, int textureUnit) {
    if (type == standard) {
        if (textureUniform < 0) {
            cerr << "Cannot bind texture unit, when no matching uniform is "
                "defined." << endl;
        }
        shaderProgram->setUniformValue(textureUniform, textureUnit);
    }
    else if (type == utility) {
        if (utilityTextureUniform < 0) {
            cerr << "Cannot bind utility texture unit, when no matching "
                "uniform is defined." << endl;
        }
        shaderProgram->setUniformValue(utilityTextureUniform, textureUnit);
    }
}

void ShaderProgram::setMatrices(QMatrix4x4 vpMatrix, QMatrix4x4 mMatrix) {
    shaderProgram->setUniformValue(mvpUniform, vpMatrix * mMatrix);
    shaderProgram->setUniformValue(normalMatrixUniform, mMatrix.normalMatrix());
    shaderProgram->setUniformValue(modelUniform, mMatrix);
}

void ShaderProgram::setMatrices_no_mult(QMatrix4x4 vpMatrix,
        QMatrix4x4 mMatrix) {
    shaderProgram->setUniformValue(mvpUniform, vpMatrix);
    shaderProgram->setUniformValue(normalMatrixUniform, mMatrix.normalMatrix());
    shaderProgram->setUniformValue(modelUniform, mMatrix);
}

void ShaderProgram::enableAttribs() {
    shaderProgram->enableAttributeArray(vertexAttrib);
    shaderProgram->enableAttributeArray(normalAttrib);
    if (texCoordAttrib > 0) {
        shaderProgram->enableAttributeArray(texCoordAttrib);
    }
}

void ShaderProgram::disableAttribs() {
    shaderProgram->disableAttributeArray(vertexAttrib);
    shaderProgram->disableAttributeArray(normalAttrib);
    if (texCoordAttrib > 0) {
        shaderProgram->disableAttributeArray(texCoordAttrib);
    }
}

void ShaderProgram::setAttribFormat(AttribType attrType, GLenum eleType,
        int offset, int tupleSize, int stride) {
    if (attrType == vertex) {
        shaderProgram->setAttributeBuffer(vertexAttrib, eleType, offset,
                tupleSize, stride);
    }
    else if (attrType == normal) {
        shaderProgram->setAttributeBuffer(normalAttrib, eleType, offset,
                tupleSize, stride);
    }
    else if (attrType == texCoord) {
        if (texCoordAttrib < 0) {
            cerr << "Cannot set attribute format for undefined attributes."
                << endl;
            exit(1);
        }
        shaderProgram->setAttributeBuffer(texCoordAttrib, eleType, offset,
                tupleSize, stride);
    }
}

void ShaderProgram::bind() {
    shaderProgram->bind();
}

void ShaderProgram::release() {
    shaderProgram->release();
}

void ShaderProgram::setTime(Time time) {
    shaderProgram->setUniformValue(timeUniform, time.count());
}

void ShaderProgram::setCameraPos(QVector3D cameraPos) {
    shaderProgram ->setUniformValue(cameraPosUniform, cameraPos);
}
