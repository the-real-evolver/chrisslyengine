//------------------------------------------------------------------------------
//  meshmanager.cpp
//  (C) 2010 Christian Bleicher
//------------------------------------------------------------------------------
#include "meshmanager.h"
#include "skeletonserialiser.h"
#include "animation.h"
#include "fswrapper.h"
#include "memoryallocatorconfig.h"
#include "debug.h"
#include <string.h>
#include <stdio.h>
#include <float.h>

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
    ce_hash_table_init(&this->resources, 4U);
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
Mesh* const
MeshManager::Load(const char* const filename)
{
    Mesh* mesh = (Mesh*)ce_hash_table_find(&this->resources, filename, strlen(filename));
    if (mesh != NULL)
    {
        return mesh;
    }

    mesh = CE_NEW Mesh();
    ce_hash_table_insert(&this->resources, filename, strlen(filename), mesh);

    unsigned int bytesPerVertex = 0U;
    unsigned char currentChunk = 0U;
    unsigned int vertexCount = 0U;
    HardwareVertexBuffer* vertexBuffer = NULL;
    Animation* animation = NULL;
    VertexAnimationTrack* animationTrack = NULL;
    char stringBuffer[256U] = {'\0'};
    String materialName;
    unsigned char materialNameLength = 0U;
    String skeletonPath;
    unsigned char skeletonPathLength = 0U;

    FileHandle fd = FSWrapper::Open(filename, ReadAccess, Random, 0777);

    while (FSWrapper::Read(fd, &currentChunk, 1U) > 0)
    {
        switch (currentChunk)
        {
            case M_SUBMESH:
                {
                    // read material name
                    FSWrapper::Read(fd, &materialNameLength, 1U);
                    FSWrapper::Read(fd, &stringBuffer, materialNameLength);
                    materialName.Set(stringBuffer, materialNameLength);

                    // read vertex count
                    FSWrapper::Read(fd, &vertexCount, 4U);

                    // read bytes per vertex
                    FSWrapper::Read(fd, &bytesPerVertex, 4U);

                    // create the vertexbuffer
                    vertexBuffer = CE_NEW HardwareVertexBuffer(vertexCount, bytesPerVertex, HBU_STATIC, false);

                    // read vertex buffer
                    if (vertexCount > 0U)
                    {
                        void* buffer = vertexBuffer->Map();
                        FSWrapper::Read(fd, buffer, vertexCount * bytesPerVertex);
                        vertexBuffer->Unmap();
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
                    float boundingRadius = FLT_MAX;
                    FSWrapper::Read(fd, &boundingRadius, 4U);
                    mesh->boundingRadius = boundingRadius;
                }
                break;
            case M_ANIMATION:
                {
                    // read animation length
                    float animLength = 0.0f;
                    FSWrapper::Read(fd, &animLength, 4U);
                    animation = mesh->CreateAnimation("default", animLength);
                }
                break;
            case M_ANIMATION_TRACK:
                {
                    // read animation track handle
                    CE_ASSERT(animation != NULL, "MeshManager::Load(): can't create VertexAnimationTrack without Animation\n");
                    unsigned char handle = 0U;
                    FSWrapper::Read(fd, &handle, 1U);
                    animationTrack = animation->CreateVertexTrack(handle);
                }
                break;
            case M_ANIMATION_MORPH_KEYFRAME:
                {
                    // read key time
                    float keyTime;
                    FSWrapper::Read(fd, &keyTime, 4U);

                    // read vertex count
                    FSWrapper::Read(fd, &vertexCount, 4U);
                    CE_ASSERT(vertexCount > 0, "MeshManager::Load(): vertex count of morph keyframe is zero\n");

                    // read bytes per vertex
                    FSWrapper::Read(fd, &bytesPerVertex, 4U);

                    // create the vertexbuffer
                    vertexBuffer = CE_NEW HardwareVertexBuffer(vertexCount, bytesPerVertex, HBU_STATIC, true);

                    // read vertex buffer
                    void* buffer = vertexBuffer->Map();
                    FSWrapper::Read(fd, buffer, vertexCount * bytesPerVertex);
                    vertexBuffer->Unmap();

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
            case M_MESH_SKELETON_FILE:
                FSWrapper::Read(fd, &skeletonPathLength, 1U);
                FSWrapper::Read(fd, &stringBuffer, skeletonPathLength);
                skeletonPath.Set(stringBuffer, skeletonPathLength);
                ce_graphics_import_skeleton(skeletonPath.C_Str(), mesh);
                break;
        }
    }

    FSWrapper::Close(fd);

    return mesh;
}

//------------------------------------------------------------------------------
/**
*/
Mesh* const
MeshManager::CreateManual(const char* const name)
{
    Mesh* mesh = (Mesh*)ce_hash_table_find(&this->resources, name, strlen(name));
    if (mesh != NULL)
    {
        CE_ASSERT(false, "MeshManager::CreateManual(): manual mesh '%s' already exists, duplicates are not allowed\n", name);
        return NULL;
    }

    mesh = CE_NEW Mesh();
    ce_hash_table_insert(&this->resources, name, strlen(name), mesh);

    return mesh;
}

//------------------------------------------------------------------------------
/**
*/
void
MeshManager::RemoveAll()
{
    unsigned int i;
    for (i = 0U; i < this->resources.bucket_count; ++i)
    {
        ce_linked_list* it = this->resources.buckets[i];
        while (it != NULL)
        {
            CE_DELETE (Mesh*)((ce_key_value_pair*)it->data)->value;
            it = it->next;
        }
    }
    ce_hash_table_clear(&this->resources);
}

//------------------------------------------------------------------------------
/**
*/
ce_hash_table* const
MeshManager::GetAllMeshes() const
{
    return &this->resources;
}

} // namespace graphics
} // namespace chrissly
