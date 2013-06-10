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
TextureManager::TextureManager()
{
    Singleton = this;
    HashTableInit(&this->resources, 4);
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
    Texture* texture = (Texture*)HashTableFind(&this->resources, name);
    if (texture != NULL)
    {
        return texture;
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
    
    texture = CE_NEW Texture();
    texture->SetFormat((PixelFormat)format);
    texture->SetWidth(width);
    texture->SetHeight(height);
    texture->SetSwizzleEnabled(1 == swizzled ? true : false);
    texture->SetBuffer(textureBuffer);
    texture->CreateInternalResourcesImpl();

    HashTableInsert(&this->resources, name, texture);

    return texture;
}

//------------------------------------------------------------------------------
/**
*/
void
TextureManager::RemoveAll()
{
    unsigned int i;
    for (i = 0; i < this->resources.capacity; i++)
    {
        LinkedList* it = ((Chain*)DynamicArrayGet(&this->resources.entries, i))->list;
        while (it != NULL)
        {
            Texture* texture = (Texture*)((KeyValuePair*)it->data)->value;
            CE_FREE((void*)texture->GetBuffer());
            CE_DELETE texture;
            it = it->next;
        }
    }

    HashTableClear(&this->resources);
}

} // namespace graphics
} // namespace chrissly
