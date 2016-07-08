#ifndef MATERIAL_H_
#define MATERIAL_H_
//------------------------------------------------------------------------------
/**
    @class chrissly::graphics::Material

    (C) 2010 Christian Bleicher
*/
#include "dynamicarray.h"
#include "pass.h"

//------------------------------------------------------------------------------
namespace chrissly
{
namespace graphics
{

class Material
{
public:
    /// default constructor
    Material();
    /// destructor
    ~Material();

    /// creates a new Pass for this Material
    Pass* CreatePass();
    /// retrieves the Pass with the given index
    Pass* GetPass(unsigned short index) const;
    /// retrieves the number of passes
    unsigned short GetNumPasses() const;
    /// removes all Passes from this Material 
    void RemoveAllPasses();

    /// loads the material, if it is not already (e.g. loads all textures)
    void Load();

private:
    /// list of primary passes
    mutable DynamicArray passes;

    bool loaded;
};

} // namespace graphics
} // namespace chrissly
//------------------------------------------------------------------------------
#endif

