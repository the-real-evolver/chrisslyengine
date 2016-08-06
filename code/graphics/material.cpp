//------------------------------------------------------------------------------
//  material.cpp
//  (C) 2010 Christian Bleicher
//------------------------------------------------------------------------------
#include "material.h"
#include "memoryallocatorconfig.h"

namespace chrissly
{
namespace graphics
{

//------------------------------------------------------------------------------
/**
*/
Material::Material() :
    loaded(false)
{
    ce_dynamic_array_init(&this->passes, 1);
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
    Pass* pass = CE_NEW Pass(this->passes.size);
    ce_dynamic_array_push_back(&this->passes, pass);

    return pass;
}

//------------------------------------------------------------------------------
/**
*/
Pass*
Material::GetPass(unsigned short index) const
{
    return (Pass*)ce_dynamic_array_get(&this->passes, index);
}

//------------------------------------------------------------------------------
/**
*/
unsigned short
Material::GetNumPasses() const
{
    return this->passes.size;
}

//------------------------------------------------------------------------------
/**
*/
void
Material::RemoveAllPasses()
{
    unsigned int i;
    for (i = 0; i < this->passes.size; ++i)
    {
        CE_DELETE (Pass*)ce_dynamic_array_get(&this->passes, i);
    }

    ce_dynamic_array_delete(&this->passes);
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
        for (i = 0; i < this->passes.size; ++i)
        {
            ((Pass*)ce_dynamic_array_get(&this->passes, i))->_Load();
        }
        this->loaded = true;
    }
}

} // namespace graphics
} // namespace chrissly
