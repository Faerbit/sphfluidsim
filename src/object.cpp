#include "object.h"

using namespace std;

Object::Object() {
    parent = nullptr;
}

Object::Object(Mesh mesh, Texture texture) {
    this->mesh = mesh;
    this->texture = texture;
    utilityTexture = Texture();
    parent = nullptr;
    position = QVector3D();
    rotation = QQuaternion();
    scale = 1.0f;
    hasUtilityTexture = false;
}

Object::Object(string meshFilePath, string textureFilePath):
    Object(Mesh(meshFilePath), Texture(textureFilePath)) {}

Object::Object(Mesh mesh, Texture texture, Texture utilityTexture) :
    Object(mesh, texture) {
    this->utilityTexture = utilityTexture;
    hasUtilityTexture = true;
}

Object::Object(std::string meshFilePath, std::string textureFilePath,
            std::string utilityTextureFilePath) :
    Object(Mesh(meshFilePath), Texture(textureFilePath),
            Texture(utilityTextureFilePath)) {}

void Object::setParent(shared_ptr<Object> parent) {
    this->parent = parent;
}

void Object::setPosition(QVector3D position) {
    this->position = position;
}

void Object::setRotation(QQuaternion rotation) {
    this->rotation = rotation.normalized();
}

QVector3D Object::getPosition() {
    return position;
}

void Object::setScale(float scale) {
    this->scale = scale;
}

void Object::render(ShaderProgram* shaderProgram, QMatrix4x4 vpMatrix,
        bool renderTextures) {
    texture.bind(0);
    if (hasUtilityTexture) {
        utilityTexture.bind(1);
    }
    QMatrix4x4 modelMatrix;
    modelMatrix.translate(position.x(), position.y(), position.z());
    if (parent != nullptr) {
        QVector3D parent_pos = parent->getPosition();
        modelMatrix.translate(parent_pos.x(), parent_pos.y(), parent_pos.z());
    }
    modelMatrix.rotate(rotation);
    modelMatrix.scale(scale);
    shaderProgram->setMatrices(vpMatrix, modelMatrix);
    if (renderTextures) {
        shaderProgram->bindTextureUnit(ShaderProgram::standard, 0);
        if (hasUtilityTexture) {
            shaderProgram->bindTextureUnit(ShaderProgram::utility, 1);
        }
    }
    mesh.render(shaderProgram, renderTextures);
    texture.release();
}
