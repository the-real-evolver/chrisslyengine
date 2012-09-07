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
Material::Material() : numPasses(0)
{
    DynamicArrayInit(&this->passes, 0);
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
    Pass* pass = CE_NEW Pass(this->numPasses);
    
    if (!DynamicArraySet(&this->passes, this->numPasses, pass))
    {
        return NULL;
    }
    
    this->numPasses++;

    return pass;
}

//------------------------------------------------------------------------------
/**
*/
Pass*
Material::GetPass(unsigned short index)
{
    return (Pass*)DynamicArrayGet(&this->passes, index);
}
    
//------------------------------------------------------------------------------
/**
*/
unsigned short
Material::GetNumPasses() const
{
    return this->numPasses;
}

//------------------------------------------------------------------------------
/**
*/
void
Material::RemoveAllPasses()
{
    unsigned int i;
    for (i = 0; i < this->numPasses; i++)
    {
        CE_DELETE (Pass*)DynamicArrayGet(&this->passes, i);	
    }

    DynamicArrayDelete(&this->passes);
    
    this->numPasses = 0;
}

} // namespace graphics
} // namespace chrissly
