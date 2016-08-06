#ifndef TEXTUREMANAGER_H_
#define TEXTUREMANAGER_H_
//------------------------------------------------------------------------------
/**
    @class chrissly::graphics::TextureManager

    (C) 2010 Christian Bleicher
*/
#include "texture.h"
#include "hashtable.h"

//------------------------------------------------------------------------------
namespace chrissly
{
namespace graphics
{

class TextureManager
{
public:
    /// get pointer to the singleton
    static TextureManager* Instance()
    {
        return Singleton;
    }

    /// default constructor
    TextureManager();
    /// destructor
    ~TextureManager();
    /// loads a texture from a file
    Texture* Load(const char* name);
    /// removes all resources
    void RemoveAll();

private:
    /// copy constructor
    TextureManager(const TextureManager&cc) {};

    static TextureManager* Singleton;

    ce_hash_table resources;
};

} // namespace graphics
} // namespace chrissly
//------------------------------------------------------------------------------
#endif
