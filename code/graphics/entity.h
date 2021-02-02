#ifndef ENTITY_H_
#define ENTITY_H_
//------------------------------------------------------------------------------
/**
    @class chrissly::graphics::Entity

    (C) 2011 Christian Bleicher
*/
#include "subentity.h"
#include "mesh.h"
#include "scenenode.h"
#include "animationstate.h"

//------------------------------------------------------------------------------
namespace chrissly
{
namespace graphics
{
class SubEntity;
class SceneNode;

class Entity
{
    friend class SceneManager;
public:
    /// destructor
    ~Entity();
    /// gets the Mesh that this Entity is based on
    const Mesh* const GetMesh() const;
    /// gets a pointer to a SubEntity, ie a part of an Entity
    SubEntity* const GetSubEntity(unsigned int index) const;
    /// retrieves the number of SubEntity objects making up this entity
    unsigned int GetNumSubEntities() const;
    /// tells this Entity whether to be visible or not
    void SetVisible(bool vis);
    /// returns whether or not this Entity is supposed to be visible
    bool IsVisible() const;
    /// sets whether or not this object will cast shadows
    /**
        @remarks
        This setting simply allows you to turn on/off shadows for a given object.
        An object will not cast shadows unless the scene supports it in any case
        (see SceneManager::SetShadowTechnique).
    */
    void SetCastShadows(bool enabled);
    /// returns whether shadow casting is enabled for this object
    bool GetCastShadows() const;
    /// sets whether or not this object will receive shadows
    void SetReceivesShadows(bool enabled);
    /// returns whether the Material of any Renderable that this MovableObject will add to the render queue will receive shadows 
    bool GetReceivesShadows() const;
    /// returns whether or not this entity is morph animated
    bool HasVertexAnimation() const;
    /// for entities based on animated meshes, gets the AnimationState object for a single animation
    /**
        @remarks
        You animate an entity by updating the animation state objects. Each of these represents the
        current state of each animation available to the entity. The AnimationState objects are
        initialised from the Mesh object.
    */
    AnimationState* const GetAnimationState(const char* const name) const;
    /// perform all the updates required for an animated entity
    void UpdateAnimation();
    /// internal method called to notify the object that it has been attached to a node
    void _NotifyAttached(SceneNode* const parent);

private:
    /// private constructor - construct from mesh (the usual constructor used)
    Entity(Mesh* const mesh);
    /// builds a list of SubEntities based on the SubMeshes contained in the Mesh
    void BuildSubEntities();

    /// node to which this object is attached
    SceneNode* parentNode;
    /// the Mesh that this Entity is based on
    Mesh* mesh;
    /// list of SubEntities (point to SubMeshes)
    mutable ce_dynamic_array subEntities;
    /// is this Entity visible?
    bool visible;
    /// does this object cast shadows?
    bool castShadows;
    /// does this object receive shadows?
    bool receivesShadows;
    /// state of animation for animable meshes
    mutable ce_hash_table animationStates;
};

} // namespace graphics
} // namespace chrissly
//------------------------------------------------------------------------------
#endif
