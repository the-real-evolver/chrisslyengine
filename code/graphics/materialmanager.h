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
    static MaterialManager* const Instance()
    {
        return Singleton;
    }

    /// default constructor
    MaterialManager();
    /// destructor
    ~MaterialManager();
    /// create all materials defined in the default material file
    void Initialise();
    /// parse material script and create materials
    void ParseScript(const char* const name);
    /// create a new material (name has to be unique)
    Material* const Create(const char* const name);
    /// retrieves a pointer to a material by name, or null if the resource does not exist
    Material* const GetByName(const char* const name);
    /// removes all resources
    void RemoveAll();
    /// returns a pointer to the hashtable that contains all registered scripts
    ce_hash_table* const GetAllScripts() const;

private:
    /// copy constructor
    MaterialManager(const MaterialManager&);
    /// prevent copy by assignment
    MaterialManager& operator = (const MaterialManager&);

    static MaterialManager* Singleton;

    ce_hash_table resources;
    mutable ce_hash_table scripts;
    MaterialParser parser;
};

} // namespace graphics
} // namespace chrissly
//------------------------------------------------------------------------------
#endif
