#ifndef MATERIALMANAGER_H_
#define MATERIALMANAGER_H_
//------------------------------------------------------------------------------
/**
    @class chrissly::graphics::MaterialManager

    (C) 2013 Christian Bleicher
*/
#include "material.h"
#include "hashtable.h"

//------------------------------------------------------------------------------
namespace chrissly
{
namespace graphics
{

class MaterialManager
{
public:
    /// get pointer to the singleton
    static MaterialManager* Instance()
    {
        return Singleton;
    }

    /// default constructor
    MaterialManager();
    /// destructor
    ~MaterialManager();
    /// creates a new blank material, but does not immediately load it
    Material* Create(const char* name);
    /// removes all resources
    void RemoveAll();

private:
    /// copy constructor
    MaterialManager(const MaterialManager&cc) {};

    static MaterialManager* Singleton;

    chrissly::core::HashTable resources;
};

} // namespace graphics
} // namespace chrissly
//------------------------------------------------------------------------------
#endif
