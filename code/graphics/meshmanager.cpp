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
    ce_hash_table_init(&this->resources, 4);
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
    Mesh* mesh = (Mesh*)ce_hash_table_find(&this->resources, filename);
    if (mesh != NULL)
    {
        return mesh;
    }

    mesh = CE_NEW Mesh();
    ce_hash_table_insert(&this->resources, filename, mesh);

    unsigned int bytesPerVertex = 36;
    unsigned char currentChunk = 0;
    unsigned int vertexCount = 0;
    HardwareVertexBuffer* vertexBuffer = NULL;
    Animation* animation = NULL;
    VertexAnimationTrack* animationTrack = NULL;
    char stringBuffer[256] = {'\0'};
    String materialName;
    unsigned char materialNameLength = 0;

    FileHandle fd = FSWrapper::Open(filename, ReadAccess, Random, 0777);

    while (FSWrapper::Read(fd, &currentChunk, 1) > 0)
    {
        switch (currentChunk)
        {
            case M_SUBMESH:
                {
                    // read material name
                    FSWrapper::Read(fd, &materialNameLength, 1);
                    FSWrapper::Read(fd, &stringBuffer, materialNameLength);
                    materialName.Set(stringBuffer, materialNameLength);

                    // read vertex count
                    FSWrapper::Read(fd, &vertexCount, 4);

                    // create the vertexbuffer
                    vertexBuffer = CE_NEW HardwareVertexBuffer(vertexCount, bytesPerVertex, HBU_STATIC);

                    // read vertex buffer
                    if (vertexCount > 0)
                    {
                        void* buffer = vertexBuffer->Lock();
                        FSWrapper::Read(fd, buffer, vertexCount * bytesPerVertex);
                        vertexBuffer->Unlock();
                    }

                    // add submesh
                    SubMesh* subMesh = mesh->CreateSubMesh();
                    subMesh->vertexData = CE_NEW VertexData(vertexBuffer);
                    subMesh->SetMaterialName(materialName);
                }
                break;
            case M_MESH_BOUNDS:
                {
                    // read bounding radius
                    float boundingRadius;
                    FSWrapper::Read(fd, &boundingRadius, 4);
                    mesh->boundingRadius = boundingRadius;
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
                    CE_ASSERT(animation != NULL, "MeshManager::Load(): can't create VertexAnimationTrack without Animation\n");
                    unsigned char handle = 0;
                    FSWrapper::Read(fd, &handle, 1);
                    animationTrack = animation->CreateVertexTrack(handle);
                }
                break;
            case M_ANIMATION_MORPH_KEYFRAME:
                {
                    // read key time
                    float keyTime;
                    FSWrapper::Read(fd, &keyTime, 4);

                    // read vertex count
                    FSWrapper::Read(fd, &vertexCount, 4);

                    // create the vertexbuffer
                    vertexBuffer = CE_NEW HardwareVertexBuffer(vertexCount, bytesPerVertex, HBU_DYNAMIC);

                    // read vertex buffer
                    void* buffer = vertexBuffer->Lock();
                    FSWrapper::Read(fd, buffer, vertexCount * bytesPerVertex);
                    vertexBuffer->Unlock();

                    // add morphkeyframe
                    CE_ASSERT(animationTrack != NULL, "MeshManager::Load(): can't create VertexMorphKeyFrame without VertexAnimationTrack\n");
                    VertexMorphKeyFrame* vertexMorphKeyFrame = animationTrack->CreateVertexMorphKeyFrame(keyTime);
                    vertexMorphKeyFrame->vertexData = CE_NEW VertexData(vertexBuffer);

                    // initialise submesh
                    SubMesh* subMesh = mesh->GetSubMesh(animationTrack->GetHandle());
                    CE_ASSERT(subMesh != NULL, "MeshManager::Load(): no submesh to map to\n");
                    subMesh->vertexAnimationType = VAT_MORPH;
                    subMesh->vertexData->vertexBuffer->SetNumVertices(vertexCount);
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
    for (i = 0; i < this->resources.bucket_count; ++i)
    {
        ce_linked_list* it = ce_hash_table_begin(&this->resources, i);
        while (it != NULL)
        {
            CE_DELETE (Mesh*)((ce_key_value_pair*)it->data)->value;
            it = it->next;
        }
    }

    ce_hash_table_clear(&this->resources);
}

} // namespace graphics
} // namespace chrissly
