#ifndef OBJECT_H
#define OBJECT_H

#include "texture.h"
#include "mesh.h"
#include "shaderprogram.h"
#include <QMatrix4x4>
#include <QVector3D>
#include <QQuaternion>

class Object {
public:
    Object();
    Object(Mesh mesh, Texture texture);
    Object(std::string meshFilePath, std::string textureFilePath);
    Object(Mesh mesh, Texture texture, Texture utilityTexture);
    Object(std::string meshFilePath, std::string textureFilePath,
            std::string utilityTextureFilePath);
    void render(ShaderProgram* shaderProgram, QMatrix4x4 vpMatrix, bool renderTextures);
    void setParent(std::shared_ptr<Object> parent);
    void setPosition(QVector3D position);
    void setRotation(QQuaternion rotation);
    QVector3D getPosition();
    void setScale(float scale);

private:
    Mesh mesh;
    Texture texture;
    Texture utilityTexture;
    std::shared_ptr<Object> parent;
    QVector3D position;
    QQuaternion rotation;
    float scale;
    bool hasUtilityTexture;
};

#endif // OBJECT_H
