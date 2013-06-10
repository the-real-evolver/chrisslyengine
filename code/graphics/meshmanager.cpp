//------------------------------------------------------------------------------
//  meshmanager.cpp
//  (C) 2010 Christian Bleicher
//------------------------------------------------------------------------------
#include "meshmanager.h"
#include "animation.h"
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
MeshManager::MeshManager()
{
    Singleton = this;
    HashTableInit(&this->resources, 4);
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
    Mesh* mesh = (Mesh*)HashTableFind(&this->resources, filename);
    if (mesh != NULL)
    {
        return mesh;
    }

    mesh = CE_NEW Mesh();
    HashTableInsert(&this->resources, filename, mesh);

    unsigned int bytesToRead = 0;
    unsigned int bytesPerVertex = 36;
    unsigned char currentChunk;
    unsigned int vertexCount = 0;
    void* vertexBuffer = NULL;
    Animation* animation = NULL;
    VertexAnimationTrack* animationTrack = NULL;

    FileHandle fd = FSWrapper::Open(filename, ReadAccess, 0777);

    while (FSWrapper::Read(fd, &currentChunk, 1) > 0)
    {
        switch (currentChunk)
        {
            case M_SUBMESH:
                {
                    // read vertex count
                    FSWrapper::Read(fd, &vertexCount, 4);

                    // read vertex buffer
                    if (vertexCount > 0)
                    {
                        bytesToRead = vertexCount * bytesPerVertex;
                        vertexBuffer = CE_MALLOC_ALIGN(16, bytesToRead);
                        CE_ASSERT(vertexBuffer != NULL, "MeshManager::Load(): failed to allocate '%i' bytes for SubMesh", bytesToRead);
                        FSWrapper::Read(fd, vertexBuffer, bytesToRead);
                    }
                    else
                    {
                        vertexBuffer = NULL;
                    }

                    // add submesh
                    SubMesh* subMesh = mesh->CreateSubMesh();
                    subMesh->vertexData = CE_NEW VertexData(vertexCount, vertexBuffer, bytesPerVertex);
                }
                break;
            case M_ANIMATION:
                {
                    // read animation length
                    float animLength;
                    FSWrapper::Read(fd, &animLength, 4);
                    animation = mesh->CreateAnimation("default", animLength);
                }
                break;
            case M_ANIMATION_TRACK:
                {
                    // read animation track
                    CE_ASSERT(animation != NULL, "MeshManager::Load(): can't create VertexAnimationTrack without Animation");
                    animationTrack = animation->CreateVertexTrack();
                }
                break;
            case M_ANIMATION_MORPH_KEYFRAME:
                {
                    // read key time
                    float keyTime;
                    FSWrapper::Read(fd, &keyTime, 4);

                    // read vertex count
                    FSWrapper::Read(fd, &vertexCount, 4);

                    // read vertex buffer
                    bytesToRead = vertexCount * bytesPerVertex;
                    vertexBuffer = CE_MALLOC_ALIGN(16, bytesToRead);
                    CE_ASSERT(vertexBuffer != NULL, "MeshManager::Load(): failed to allocate '%i' bytes for VertexMorphKeyFrame", bytesToRead);
                    FSWrapper::Read(fd, vertexBuffer, bytesToRead);

                    // add morphkeyframe
                    CE_ASSERT(animationTrack != NULL, "MeshManager::Load(): can't create VertexMorphKeyFrame without VertexAnimationTrack");
                    VertexMorphKeyFrame* vertexMorphKeyFrame = animationTrack->CreateVertexMorphKeyFrame(keyTime);
                    vertexMorphKeyFrame->vertexData = CE_NEW VertexData(vertexCount, vertexBuffer, bytesPerVertex);

                    // Fixme: setup for multiple animationtracks
                    SubMesh* subMesh = mesh->GetSubMesh(0);
                    CE_ASSERT(subMesh != NULL, "MeshManager::Load(): no submesh to map to");
                    subMesh->vertexAnimationType = VAT_MORPH;
                    subMesh->vertexData->vertexCount = vertexCount;
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
    unsigned int i;
    for (i = 0; i < this->resources.capacity; i++)
    {
        LinkedList* it = ((Chain*)DynamicArrayGet(&this->resources.entries, i))->list;
        while (it != NULL)
        {
            CE_DELETE (Mesh*)((KeyValuePair*)it->data)->value;
            it = it->next;
        }
    }

    HashTableClear(&this->resources);
}

} // namespace graphics
} // namespace chrissly
