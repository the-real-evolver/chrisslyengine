//------------------------------------------------------------------------------
//  meshmanager.cpp
//  (C) 2010 Christian Bleicher
//------------------------------------------------------------------------------
#include "meshmanager.h"
#include "fswrapper.h"
#include "memoryallocatorconfig.h"
#include "debug.h"
#include <string.h>
#include <stdio.h>

namespace chrissly
{
namespace graphics
{

using namespace chrissly::core;

MeshManager* MeshManager::Singleton = NULL;

//------------------------------------------------------------------------------
/**
*/
MeshManager::MeshManager() : resources(NULL)
{
    Singleton = this;
}

//------------------------------------------------------------------------------
/**
*/
MeshManager::~MeshManager()
{
    Singleton = NULL;
    this->RemoveAll();
}

//------------------------------------------------------------------------------
/**
*/
Mesh*
MeshManager::Load(const char* filename)
{
    LinkedList* it = this->resources;
    while (it != NULL)
    {
        if (0 == strcmp(filename, ((MeshResource*)it->data)->fileName))
        {
            return ((MeshResource*)it->data)->mesh;
        }
        it = it->next;
    }
    
    Mesh* mesh = CE_NEW Mesh();
    MeshResource* meshResource = CE_NEW MeshResource;
    meshResource->mesh = mesh;
    meshResource->fileName = filename;
    this->resources = linkedlistAdd(&this->resources, meshResource);
    this->resources->data = meshResource;
  
    unsigned int bytesToRead = 0;
    unsigned int bytesPerVertex = 36;
    unsigned char currentChunk;
    unsigned char vertexCount[4];
    void* vertexBuffer = NULL;
    
    core::FileHandle fd = FSWrapper::Open(filename, ReadAccess, 0777);

    while (FSWrapper::Read(fd, &currentChunk, 1) > 0)
    {
        switch (currentChunk)
        {
            case M_SUBMESH:
                {
                    // read vertex count
                    FSWrapper::Read(fd, vertexCount, 4);

                    // read vertex buffer
                    bytesToRead = *(unsigned int*)vertexCount * bytesPerVertex;
                    vertexBuffer = CE_MALLOC_ALIGN(16, bytesToRead);
                    CE_ASSERT(vertexBuffer != NULL, "MeshManager::Load(): can't allocate '%i' bytes", bytesToRead);
                    FSWrapper::Read(fd, vertexBuffer, bytesToRead);

                    // add submesh
                    SubMesh* subMesh = mesh->CreateSubMesh();
                    subMesh->_vertexCount = *(unsigned int*)vertexCount;
                    subMesh->_vertexBuffer = vertexBuffer;
                }
                break;
            case M_ANIMATION:
                break;
            case M_ANIMATION_TRACK:
                break;
            case M_ANIMATION_MORPH_KEYFRAME:
                {
                    // read vertex count
                    FSWrapper::Read(fd, vertexCount, 4);
                     
                    // read vertex buffer
                    bytesToRead = *(unsigned int*)vertexCount * bytesPerVertex;
                    vertexBuffer = CE_MALLOC_ALIGN(16, bytesToRead);
                    CE_ASSERT(vertexBuffer != NULL, "MeshManager::Load(): can't allocate '%i' bytes", bytesToRead);
                    FSWrapper::Read(fd, vertexBuffer, bytesToRead);
 
                    // add morphkeyframe
                    SubMesh* subMesh = mesh->CreateSubMesh();
                    subMesh->_vertexCount = *(unsigned int*)vertexCount;
                    subMesh->_vertexBuffer = vertexBuffer;
                }
                break;
        }
    }

    FSWrapper::Close(fd);

    return mesh;
}

//------------------------------------------------------------------------------
/**
*/
void
MeshManager::RemoveAll()
{
    LinkedList* it = this->resources;
    while (it != NULL)
    {
        LinkedList* node = it;        
        MeshResource* resource = (MeshResource*)node->data;

        CE_DELETE resource->mesh;
        CE_DELETE resource;

        it = it->next;
        linkedlistRemove(node);
    }
    this->resources = NULL;
}

} // namespace graphics
} // namespace chrissly
