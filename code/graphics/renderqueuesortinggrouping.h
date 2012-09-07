#ifndef RENDERQUEUESORTINGGROUPING_H_
#define RENDERQUEUESORTINGGROUPING_H_
//------------------------------------------------------------------------------
/**
    (C) 2011 Christian Bleicher
*/
#include "subentity.h"
#include "pass.h"

//------------------------------------------------------------------------------
namespace chrissly
{
namespace graphics
{

/** 
    Struct associating a single Pass with a single Renderable. 
    This is used to for objects sorted by depth and thus not
    grouped by pass.
*/
struct RenderablePass
{
    /// Pointer to the Renderable details
    SubEntity* renderable;
    /// Pointer to the Pass
    Pass* pass;

    RenderablePass(SubEntity* rend, Pass* p) : renderable(rend), pass(p) {}
};


} // namespace graphics
} // namespace chrissly
//------------------------------------------------------------------------------
#endif