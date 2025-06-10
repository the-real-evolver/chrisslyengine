//------------------------------------------------------------------------------
//  material.cpp
//  (C) 2010 Christian Bleicher
//------------------------------------------------------------------------------
#include "material.h"
#include "memoryallocatorconfig.h"
#include "chrisslyarray.h"

namespace chrissly
{
namespace graphics
{

//------------------------------------------------------------------------------
/**
*/
Material::Material() :
    shadowCasterMaterial(NULL),
    passes(NULL),
    loaded(false)
{
    ce_array_init(this->passes, 1U);
}

//------------------------------------------------------------------------------
/**
*/
Material::~Material()
{
    this->RemoveAllPasses();
}

//------------------------------------------------------------------------------
/**
*/
Pass*
Material::CreatePass()
{
    Pass* pass = CE_NEW Pass((unsigned short)ce_array_size(this->passes));
    ce_array_push_back(this->passes, pass);

    return pass;
}

//------------------------------------------------------------------------------
/**
*/
Pass*
Material::GetPass(unsigned short index) const
{
    return this->passes[index];
}

//------------------------------------------------------------------------------
/**
*/
unsigned short
Material::GetNumPasses() const
{
    return (unsigned short)ce_array_size(this->passes);
}

//------------------------------------------------------------------------------
/**
*/
void
Material::RemoveAllPasses()
{
    unsigned int i;
    for (i = 0U; i < ce_array_size(this->passes); ++i)
    {
        CE_DELETE this->passes[i];
    }
    ce_array_delete(this->passes);
}

//------------------------------------------------------------------------------
/**
*/
void
Material::SetShadowCasterMaterial(Material* mat)
{
    this->shadowCasterMaterial = mat;
}

//------------------------------------------------------------------------------
/**
*/
Material*
Material::GetShadowCasterMaterial() const
{
    return this->shadowCasterMaterial;
}

//------------------------------------------------------------------------------
/**
*/
void
Material::Load()
{
    if (!this->loaded)
    {
        unsigned int i;
        for (i = 0U; i < ce_array_size(this->passes); ++i)
        {
            this->passes[i]->_Load();
        }
        this->loaded = true;
    }
}

} // namespace graphics
} // namespace chrissly
