#ifndef SCENENODE_H_
#define SCENENODE_H_
//------------------------------------------------------------------------------
/**
    @class chrissly::graphics::SceneNode

    (C) 2011 Christian Bleicher
*/
#include "node.h"
#include "entity.h"

//------------------------------------------------------------------------------
namespace chrissly
{
namespace graphics
{
class SceneManager;

class SceneNode : public Node
{
    friend class SceneManager;
public:
    /// destructor
    ~SceneNode();

    /// creates a new SceneNode as a child of this node
    SceneNode* const CreateChildSceneNode();
    /// gets the parent of this scenenode
    SceneNode* const GetParentSceneNode() const;
    /// recursively removes all child nodes including attached entities and delete them
    void RemoveAndDestroyAllChildren();

    /// recursively make all objects attached to this node become visible / invisible
    void SetVisible(bool vis);

    /// adds an instance of a scene object to this node
    void AttachObject(Entity* const obj);
    /// reports the number of objects attached to this node
    unsigned short NumAttachedObjects() const;
    /// retrieves a pointer to an attached object
    /**
        @remarks
            Retrieves by index. The index of an object may change as other
            objects are added / removed.
    */
    Entity* const GetAttachedObject(unsigned short index) const;
    /// detaches the specified object from this node
    void DetachObject(Entity* const obj);
    /// detaches all objects attached to this node
    void DetachAllObjects();

private:
    /// constructor, only to be called by the creator SceneManager
    SceneNode();

    Entity** objects;
};

} // namespace graphics
} // namespace chrissly
//------------------------------------------------------------------------------
#endif
