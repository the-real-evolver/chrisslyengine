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
    /// gets a pointer to a SubEntity, ie a part of an Entity
    SubEntity* GetSubEntity(unsigned int index);
    /// retrieves the number of SubEntity objects making up this entity
    unsigned int GetNumSubEntities() const;
    /// internal method called to notify the object that it has been attached to a node
    void _NotifyAttached(SceneNode* parent);
    /// sets whether or not this object will cast shadows
    /**
        @remarks
        This setting simply allows you to turn on/off shadows for a given object.
        An object will not cast shadows unless the scene supports it in any case
        (see SceneManager::setShadowTechnique).
    */
    void SetCastShadows(bool enabled);
    /// returns whether shadow casting is enabled for this object
    bool GetCastShadows() const;
    /// sets whether or not this object will receive shadows
    void SetReceivesShadows(bool enabled);
    /// returns whether the Material of any Renderable that this MovableObject will add to the render queue will receive shadows 
    bool GetReceivesShadows() const;

private:
    /// private constructor - specify name (the usual constructor used)
    Entity(const char* name, Mesh* mesh);
    /// builds a list of SubEntities based on the SubMeshes contained in the Mesh
    void BuildSubEntityList(Mesh* mesh, DynamicArray* sublist);
    /// node to which this object is attached
    SceneNode* parentNode;
    /// the Mesh that this Entity is based on
    Mesh* mesh;
    /// list of SubEntities (point to SubMeshes)
    DynamicArray subEntityList;
    unsigned short numSubEntities;
    /// does this object cast shadows?
    bool castShadows;
    /// does this object receive shadows?
    bool receivesShadows;

};

} // namespace graphics
} // namespace chrissly
//------------------------------------------------------------------------------
#endif
