//------------------------------------------------------------------------------
//  texturemanager.cpp
//  (C) 2010 Christian Bleicher
//------------------------------------------------------------------------------
#include "texturemanager.h"
#include "fswrapper.h"
#include "memoryallocatorconfig.h"
#include "chrisslyconfig.h"
#include "debug.h"
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
Texture*
TextureManager::Load(const char* const name, TextureType type)
{
    Texture* texture = (Texture*)ce_hash_table_find(&this->resources, name, strlen(name));
    if (texture != NULL)
    {
        return texture;
    }

    const unsigned int headerSizeBytes = 7U;
    unsigned short width = 0U, height = 0U;
    unsigned char format = PF_UNKNOWN, numMipmaps = 0U, swizzled = 0U;
    unsigned int numImages = (TEX_TYPE_CUBE_MAP == type) ? 6U : 1U;
    void* imageBuffers[6U] = {NULL};
    char fileNames[6U][CE_MAX_PATH] = {{'\0'}};

    if (TEX_TYPE_2D == type)
    {
        strncpy(fileNames[0U], name, sizeof(fileNames[0U]));
    }
    else if (TEX_TYPE_CUBE_MAP == type)
    {
        char* ext = (char*)strrchr(name, '.');
        if (NULL == ext) return NULL;
        strncpy(fileNames[0U], name, ext - name); strcat(strcat(fileNames[0U], "_px"), ext);
        strncpy(fileNames[1U], name, ext - name); strcat(strcat(fileNames[1U], "_nx"), ext);
        strncpy(fileNames[2U], name, ext - name); strcat(strcat(fileNames[2U], "_py"), ext);
        strncpy(fileNames[3U], name, ext - name); strcat(strcat(fileNames[3U], "_ny"), ext);
        strncpy(fileNames[4U], name, ext - name); strcat(strcat(fileNames[4U], "_pz"), ext);
        strncpy(fileNames[5U], name, ext - name); strcat(strcat(fileNames[5U], "_nz"), ext);
    }
    else
    {
        CE_ASSERT(false, "TextureManager::Load(): unsupported texture type\n");
        return NULL;
    }

    unsigned int i;
    for (i = 0U; i < numImages; ++i)
    {
        FileHandle fd = FSWrapper::Open(fileNames[i], READ_ACCESS, RANDOM, 0777);
        unsigned int fileSize = FSWrapper::GetFileSize(fd);
        FSWrapper::Read(fd, &format, 1U);
        FSWrapper::Read(fd, &width, 2U);
        FSWrapper::Read(fd, &height, 2U);
        FSWrapper::Read(fd, &numMipmaps, 1U);
        FSWrapper::Read(fd, &swizzled, 1U);
        imageBuffers[i] = CE_MALLOC_ALIGN(CE_CACHE_LINE_SIZE, fileSize - headerSizeBytes);
        FSWrapper::Read(fd, imageBuffers[i], fileSize - headerSizeBytes);
        FSWrapper::Close(fd);
    }

    if (TEX_TYPE_2D == type)
    {
        texture = CE_NEW Texture();
    }
    else if (TEX_TYPE_CUBE_MAP == type)
    {
        texture = CE_NEW Texture(imageBuffers);
    }
    texture->SetFormat((PixelFormat)format);
    texture->SetWidth(width);
    texture->SetHeight(height);
    texture->SetNumMipmaps(numMipmaps);
    texture->SetSwizzleEnabled(1U == swizzled);
    texture->SetBuffer((TEX_TYPE_CUBE_MAP == type) ? NULL : imageBuffers[0U]);
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
        ce_linked_list* it = this->resources.buckets[i];
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
