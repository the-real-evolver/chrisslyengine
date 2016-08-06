#ifndef MATERIALMANAGER_H_
#define MATERIALMANAGER_H_
//------------------------------------------------------------------------------
/**
    @class chrissly::graphics::MaterialManager

    (C) 2013 Christian Bleicher
*/
#include "materialparser.h"
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
    /// create all materials defined in the material file
    void Initialise();
    /// parse material script and create the material
    void ParseScript(const char* name);
    /// create a new material, or retrieve an existing one with the same name if it already exists
    Material* CreateOrRetrieve(const char* name);
    /// retrieves a pointer to a material by name, or null if the resource does not exist
    Material* GetByName(const char* name);
    /// removes all resources
    void RemoveAll();

private:
    /// copy constructor
    MaterialManager(const MaterialManager&cc) {};

    static MaterialManager* Singleton;

    ce_hash_table resources;
    MaterialParser parser;
};

} // namespace graphics
} // namespace chrissly
//------------------------------------------------------------------------------
#endif
