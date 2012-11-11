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

    FileHandle fd = FSWrapper::Open(name, ReadAccess, 0777);
    unsigned int fileSize = FSWrapper::GetFileSize(fd);
    unsigned int headerSizeBytes = 6;

    unsigned char format = PF_UNKNOWN;
    unsigned short width = 0;
    unsigned short height = 0;
    unsigned char swizzled = 0;

    FSWrapper::Read(fd, &format, 1);
    FSWrapper::Read(fd, &width, 2);
    FSWrapper::Read(fd, &height, 2);
    FSWrapper::Read(fd, &swizzled, 1);

    void* textureBuffer = CE_MALLOC_ALIGN(16, fileSize - headerSizeBytes);
    FSWrapper::Read(fd, textureBuffer, fileSize - headerSizeBytes);
    FSWrapper::Close(fd);
    
    Texture* texture = CE_NEW Texture();
    texture->SetFormat((PixelFormat)format);
    texture->SetWidth(width);
    texture->SetHeight(height);
    texture->SetSwizzleEnabled(1 == swizzled ? true : false);
    texture->SetBuffer(textureBuffer);
    texture->CreateInternalResourcesImpl();
    
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
