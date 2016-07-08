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
    DynamicArrayInit(&this->passes, 1);
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
    DynamicArrayPushBack(&this->passes, pass);

    return pass;
}

//------------------------------------------------------------------------------
/**
*/
Pass*
Material::GetPass(unsigned short index) const
{
    return (Pass*)DynamicArrayGet(&this->passes, index);
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
        CE_DELETE (Pass*)DynamicArrayGet(&this->passes, i);
    }

    DynamicArrayDelete(&this->passes);
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
            ((Pass*)DynamicArrayGet(&this->passes, i))->_Load();
        }
        this->loaded = true;
    }
}

} // namespace graphics
} // namespace chrissly
