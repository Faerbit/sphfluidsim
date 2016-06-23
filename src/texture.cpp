#include "texture.h"
#include <iostream>
#include "constants.h"

using namespace std;

Texture::Texture(string filePath) {
    ptr = TextureManager::load(filePath);
}

Texture::Texture() {
    ptr = nullptr;
}

void Texture::bind(int textureUnit) {
    ptr->bind(textureUnit);
}

void Texture::release() {
    ptr->release();
}

void Texture::setWrapMode(QOpenGLTexture::WrapMode mode) {
    ptr->setWrapMode(mode);
}

sharedTexture Texture::TextureManager::loadResource (string filePath) {
    sharedTexture ret = sharedTexture(new QOpenGLTexture(
            QImage((TEXTURE_PATH + filePath).c_str()).mirrored()));
    if (ret->textureId() == 0) {
        cerr << "Texture " << TEXTURE_PATH + filePath
            << " could not be loaded. Exiting."
            << endl;
        exit(1);
    }
    ret->setMinificationFilter(QOpenGLTexture::LinearMipMapLinear);
    ret->setMagnificationFilter(QOpenGLTexture::Linear);
    return ret;
}
