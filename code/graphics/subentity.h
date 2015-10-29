#ifndef SUBENTITY_H_
#define SUBENTITY_H_
//------------------------------------------------------------------------------
/**
    @class chrissly::graphics::SubEntity

    (C) 2010 Christian Bleicher
*/
#include "submesh.h"
#include "material.h"

//------------------------------------------------------------------------------
namespace chrissly
{
namespace graphics
{
class Entity;

class SubEntity
{
    friend class Entity;
    friend class SceneManager;
public:
    /// destructor
    ~SubEntity();
    /// sets the name of the material to be used
    void SetMaterialName(const core::String& matName);
    /// sets a Material to be used
    void SetMaterial(Material* material); 
    /// retrieves a pointer to the material this renderable object uses
    Material* GetMaterial() const;
    /// tells this SubEntity whether to be visible or not
    void SetVisible(bool visible);
    /// returns whether or not this SubEntity is supposed to be visible 
    bool IsVisible() const;
    /// accessor method to read mesh data 
    SubMesh* GetSubMesh();
    /// advanced method to get the hardware morph vertex information
    VertexData* _GetHardwareVertexAnimVertexData() const;
    /// set vertex morph weight value
    void SetMorphWeight(float weight);
    /// get vertex morph weight value
    float GetMorphWeight() const;

private:
    /// private constructor - don't allow creation by anybody else
    SubEntity(Entity* parent, SubMesh* subMeshBasis);
    /// pointer to parent
    Entity* parentEntity;
    /// cached pointer to material 
    Material* pMaterial;
    /// submesh basis
    SubMesh* subMesh;
    /// is this SubEntity visible?
    bool visible;
    /// the vertex data for hardware morphing (pos2 binding)
    VertexData* hardwareVertexAnimVertexData;
    /// vertex morph weight
    float morphWeight;
};

} // namespace graphics
} // namespace chrissly
//------------------------------------------------------------------------------
#endif
