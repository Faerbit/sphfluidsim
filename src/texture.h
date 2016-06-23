#ifndef TEXTURE_H
#define TEXTURE_H

#include<QOpenGLTexture>
#include <memory>
#include "resourcemanager.h"

typedef std::shared_ptr<QOpenGLTexture> sharedTexture;

class Texture {
public:
    Texture();
    Texture(std::string filePath);
    void bind(int textureUnit);
    void release();
    void setWrapMode(QOpenGLTexture::WrapMode mode);

private:
    sharedTexture ptr;

    class TextureManager : public ResourceManager<QOpenGLTexture,
                                                  TextureManager> {
    friend ResourceManager<QOpenGLTexture, TextureManager>;
    private:
        static sharedTexture loadResource(std::string filePath);
    };
};

#endif // TEXTURE_H
