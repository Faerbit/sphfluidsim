/*
 * ModelLoader class
 * A simple interface to the Assimp library to generate arrays
 * to be used as Buffer Objects within OpenGL
 *
 * v0.4 beta, 2015
 *
 * kai.krueckel@alumni.fh-aachen.de
 *
 * Released under THE BEER-WARE LICENSE (Rev. 42)
 *
*/

#include "modelloader.h"
#include <iostream>

ModelLoader::ModelLoader()
    : _hasScene(false)
    , scene(NULL)
{

}

ModelLoader::~ModelLoader()
{
    /* Note: Assimp::Importer cleans up after itself, freeing the scene it previously returned */
}

bool ModelLoader::loadObjectFromFile(const std::string& pFile)
{
    /* TODO: this is some weak error handling */
    if (_hasScene) {
        return false;
    }

    // load scene with triangulation, removing identical vertices
    // and generating normals, if necessary
    scene = importer.ReadFile(pFile, aiProcess_Triangulate | aiProcess_JoinIdenticalVertices | aiProcess_GenSmoothNormals);

    // check if import was successful
    if(!scene) {
      /* TODO: do something with importer.GetErrorString() */
      return false;
    }

    // check if there is at least one mesh
    if (scene->mNumMeshes == 0 || scene->mMeshes[0]->mNumFaces == 0) {
        importer.FreeScene();
        return false;
    }

    // everything checked out
    _hasScene = true;
    return true;
}

bool ModelLoader::hasScene()
{
    return _hasScene;
}

void ModelLoader::genSimpleVBO(GLfloat* vbo, unsigned int meshId) const
{
    if (!_hasScene) {
        return;
    }
    // copy array and add 4th component
    for (unsigned int i = 0; i < scene->mMeshes[meshId]->mNumVertices; ++i) {
        vbo[i*4+0] = scene->mMeshes[meshId]->mVertices[i].x;
        vbo[i*4+1] = scene->mMeshes[meshId]->mVertices[i].y;
        vbo[i*4+2] = scene->mMeshes[meshId]->mVertices[i].z;
        vbo[i*4+3] = 1.0f;
    }
}

void ModelLoader::genVBO(GLfloat* vbo, unsigned int meshId, bool normals, bool texcoords) const
{
    if (!_hasScene) {
        return;
    }

    if (! hasTextureCoordinates(meshId) && texcoords) {
        std::cerr << "Model does not have texture coordinates "
            << "although they are requested." << std::endl;
        return;
    }

    // calculate stride
    unsigned int stride = 4;
    if (normals) {
        stride += 4;
    }
    if (texcoords) {
        stride += 4;
    }

    // copy array and add 4th component
    int unsigned n;
    for (unsigned int i = 0; i < scene->mMeshes[meshId]->mNumVertices; ++i) {
        //std::cout << "read vertex number " << i << std::endl;
        n = 0;
        vbo[i*stride+n++] = scene->mMeshes[meshId]->mVertices[i].x;
        vbo[i*stride+n++] = scene->mMeshes[meshId]->mVertices[i].y;
        vbo[i*stride+n++] = scene->mMeshes[meshId]->mVertices[i].z;
        vbo[i*stride+n++] = 1.0f;

        if (normals) {
            vbo[i*stride+n++] = scene->mMeshes[meshId]->mNormals[i].x;
            vbo[i*stride+n++] = scene->mMeshes[meshId]->mNormals[i].y;
            vbo[i*stride+n++] = scene->mMeshes[meshId]->mNormals[i].z;
            vbo[i*stride+n++] = 1.0f;
        }

        if (texcoords) {
            /* TODO: auto-selects first UV(W) channel */
            vbo[i*stride+n++] = (scene->mMeshes[meshId]->mTextureCoords[0])[i].x;
            vbo[i*stride+n++] = (scene->mMeshes[meshId]->mTextureCoords[0])[i].y;
            vbo[i*stride+n++] = (scene->mMeshes[meshId]->mTextureCoords[0])[i].z;
            //std::cout << "derp: " << scene->mMeshes[meshId]->mTextureCoords[i]->z << std::endl;
            vbo[i*stride+n++] = 1.0f;
        }
    }
}

void ModelLoader::genIndexArray(GLuint* vbi, unsigned int meshId) const
{
    if (!_hasScene) {
        return;
    }
    // copying this whole array takes time, but just referencing it and keeping the other data is worse
    for (unsigned int i = 0; i < scene->mMeshes[meshId]->mNumFaces; ++i) {
        vbi[i*3+0] = scene->mMeshes[meshId]->mFaces[i].mIndices[0];
        vbi[i*3+1] = scene->mMeshes[meshId]->mFaces[i].mIndices[1];
        vbi[i*3+2] = scene->mMeshes[meshId]->mFaces[i].mIndices[2];
    }
}

unsigned int ModelLoader::lengthOfSimpleVBO(unsigned int meshId) const
{
    if (!_hasScene) {
        return 0;
    }
    return scene->mMeshes[meshId]->mNumVertices * 4;
}

unsigned int ModelLoader::lengthOfVBO(unsigned int meshId, bool normals, bool texcoords) const
{
    unsigned int res = scene->mMeshes[meshId]->mNumVertices * 4;
    if (normals) {
        res += scene->mMeshes[meshId]->mNumVertices * 4;
    }
    if (texcoords) {
        res += scene->mMeshes[meshId]->mNumVertices * 4;
    }
    return res;
}

unsigned int ModelLoader::lengthOfIndexArray(unsigned int meshId) const
{
    if (!_hasScene || scene->mMeshes[meshId]->mNumFaces == 0) {
        return 0;
    }
    return scene->mMeshes[meshId]->mNumFaces * scene->mMeshes[meshId]->mFaces[0].mNumIndices;
}

bool ModelLoader::hasTextureCoordinates(unsigned int meshId) const
{
    /* TODO: assumes first UV(W) channel */
    return scene->mMeshes[meshId]->HasTextureCoords(0);
}


