//------------------------------------------------------------------------------
//  texturemanager.cpp
//  (C) 2010 Christian Bleicher
//------------------------------------------------------------------------------
#include "texturemanager.h"
#include "fswrapper.h"
#include "memoryallocatorconfig.h"
#include <string.h>
#include <stdio.h>

namespace chrissly
{
namespace graphics
{

using namespace chrissly::core;

TextureManager* TextureManager::Singleton = NULL;

//------------------------------------------------------------------------------
/**
*/
TextureManager::TextureManager() : resources(NULL)
{
    Singleton = this;
}

//------------------------------------------------------------------------------
/**
*/
TextureManager::~TextureManager()
{
    Singleton = NULL;
    this->RemoveAll();
}

//------------------------------------------------------------------------------
/**
*/
Texture*
TextureManager::Load(const char* name)
{
    LinkedList* it = this->resources;
    while (it != NULL)
    {
        if (0 == strcmp(name, ((TextureResource*)it->data)->fileName))
        {
            return ((TextureResource*)it->data)->texture;
        }
        it = it->next;
    }
    
    void* textureBuffer = NULL;

    core::FileHandle fd = FSWrapper::Open(name, ReadAccess, 0777);
    unsigned int fileSize = FSWrapper::GetFileSize(fd);
    textureBuffer = CE_MALLOC_ALIGN(16, fileSize);
    FSWrapper::Read(fd, textureBuffer, fileSize);
    FSWrapper::Close(fd);
    
    Texture* texture = CE_NEW Texture();
    texture->SetBuffer(textureBuffer);
    // texture->CreateInternalResourcesImpl();
    
    TextureResource* textureResource = CE_NEW TextureResource;
    textureResource->texture = texture;
    textureResource->fileName = name;
    this->resources = linkedlistAdd(&this->resources, textureResource);
    this->resources->data = textureResource;
    
    return texture;
}

//------------------------------------------------------------------------------
/**
*/
void
TextureManager::RemoveAll()
{
    LinkedList* it = this->resources;
    while (it != NULL)
    {
        LinkedList* node = it;
        
        TextureResource* resource = (TextureResource*)node->data;
        CE_FREE((void*)resource->texture->GetBuffer());
        CE_DELETE resource->texture;
        CE_DELETE resource;
        
        it = it->next;
        linkedlistRemove(node);
    }
    this->resources = NULL;
}

} // namespace graphics
} // namespace chrissly
