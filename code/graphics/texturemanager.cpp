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
    ce_hash_table_init(&this->resources, 4U);
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
Texture* const
TextureManager::Load(const char* const name)
{
    Texture* texture = (Texture*)ce_hash_table_find(&this->resources, name, strlen(name));
    if (texture != NULL)
    {
        return texture;
    }

    FileHandle fd = FSWrapper::Open(name, ReadAccess, Random, 0777);
    unsigned int fileSize = FSWrapper::GetFileSize(fd);
    const unsigned int headerSizeBytes = 7U;

    unsigned char format = PF_UNKNOWN;
    unsigned short width = 0U;
    unsigned short height = 0U;
    unsigned char numMipmaps = 0U;
    unsigned char swizzled = 0U;

    FSWrapper::Read(fd, &format, 1U);
    FSWrapper::Read(fd, &width, 2U);
    FSWrapper::Read(fd, &height, 2U);
    FSWrapper::Read(fd, &numMipmaps, 1U);
    FSWrapper::Read(fd, &swizzled, 1U);

    void* textureBuffer = CE_MALLOC_ALIGN(CE_CACHE_LINE_SIZE, fileSize - headerSizeBytes);
    FSWrapper::Read(fd, textureBuffer, fileSize - headerSizeBytes);
    FSWrapper::Close(fd);

    texture = CE_NEW Texture();
    texture->SetFormat((PixelFormat)format);
    texture->SetWidth(width);
    texture->SetHeight(height);
    texture->SetNumMipmaps(numMipmaps);
    texture->SetSwizzleEnabled(1U == swizzled);
    texture->SetBuffer(textureBuffer);
    texture->CreateInternalResources();

    ce_hash_table_insert(&this->resources, name, strlen(name), texture);

    return texture;
}

//------------------------------------------------------------------------------
/**
*/
void
TextureManager::RemoveAll()
{
    unsigned int i;
    for (i = 0U; i < this->resources.bucket_count; ++i)
    {
        ce_linked_list* it = ce_hash_table_begin(&this->resources, i);
        while (it != NULL)
        {
            CE_DELETE (Texture*)((ce_key_value_pair*)it->data)->value;
            it = it->next;
        }
    }

    ce_hash_table_clear(&this->resources);
}

} // namespace graphics
} // namespace chrissly
