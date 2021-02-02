#ifndef SCENENODE_H_
#define SCENENODE_H_
//------------------------------------------------------------------------------
/**
    @class chrissly::graphics::SceneNode

    (C) 2011 Christian Bleicher
*/
#include "linkedlist.h"
#include "dynamicarray.h"
#include "quaternion.h"
#include "vector3.h"
#include "matrix4.h"
#include "entity.h"

//------------------------------------------------------------------------------
namespace chrissly
{
namespace graphics
{
class SceneManager;

class SceneNode
{
    friend class SceneManager;
public:
    /// destructor
    ~SceneNode();
    /// creates an unnamed new SceneNode as a child of this node
    SceneNode* const CreateChildSceneNode();
    /// gets the parent of this scenenode
    SceneNode* const GetParentSceneNode() const;
    /// removes all child Nodes attached to this node
    /**
        Does not delete the nodes, just detaches them from
        this parent, potentially to be reattached elsewhere.
    */
    void RemoveAllChildren();
    /// recursively removes all child nodes including attached entities and delete them
    void RemoveAndDestroyAllChildren();

    /// sets the orientation of this node via a quaternion
    /**
    @remarks
        Orientations, unlike other transforms, are not always inherited by child nodes.
        Whether or not orientations affect the orientation of the child nodes depends on
        the setInheritOrientation option of the child. In some cases you want a orientating
        of a parent node to apply to a child node (e.g. where the child node is a part of
        the same object, so you want it to be the same relative orientation based on the
        parent's orientation), but not in other cases (e.g. where the child node is just
        for positioning another object, you want it to maintain it's own orientation).
        The default is to inherit as with other transforms.
    @par
        Note that rotations are oriented around the node's origin.
    */
    void SetOrientation(const core::Quaternion& q);
    /// sets the orientation of this node via quaternion parameters
    void SetOrientation(float w, float x, float y, float z);
    /// returns a quaternion representing the nodes orientation
    const core::Quaternion& GetOrientation() const;
    /// sets the position of the node relative to it's parent
    void SetPosition(const core::Vector3& pos);
    /// sets the position of the node relative to it's parent
    void SetPosition(float x, float y, float z);
    /// gets the position of the node relative to it's parent
    const core::Vector3& GetPosition() const;
    /// sets the scaling factor applied to this node
    /**
    @remarks
        Scaling factors, unlike other transforms, are not always inherited by child nodes.
        Whether or not scalings affect the size of the child nodes depends on the setInheritScale
        option of the child. In some cases you want a scaling factor of a parent node to apply to
        a child node (e.g. where the child node is a part of the same object, so you want it to be
        the same relative size based on the parent's size), but not in other cases (e.g. where the
        child node is just for positioning another object, you want it to maintain it's own size).
        The default is to inherit as with other transforms.
    @par
        Note that like rotations, scalings are oriented around the node's origin.
    */
    void SetScale(const core::Vector3& s);
    /// sets the scaling factor applied to this node
    void SetScale(float x, float y, float z);
    /// gets the scaling factor of this node
    const core::Vector3 & GetScale() const;
    /// rotate the node around the z-axis
    void Roll(float angle);
    /// rotate the node around the x-axis
    void Pitch(float angle);
    /// rotate the node around the y-axis
    void Yaw(float angle);
    /// rotate the node around an arbitrary axis
    void Rotate(const core::Vector3& axis, float angle);
    /// rotate the node around an aritrary axis using a Quarternion
    void Rotate(const core::Quaternion& q);

    /// recursively make all objects attached to this node become visible / invisible
    void SetVisible(bool vis);

    /// adds an instance of a scene object to this node
    /**
    @remarks
        Scene objects can include Entity objects, Camera objects, Light objects,
        ParticleSystem objects etc. Anything that subclasses from MovableObject.
    */
    void AttachObject(Entity* const obj);
    /// reports the number of objects attached to this node
    unsigned short NumAttachedObjects() const;
    /// retrieves a pointer to an attached object
    /**
    @remarks
        Retrieves by index, see alternate version to retrieve by name. The index
        of an object may change as other objects are added / removed.
    */
    Entity* const GetAttachedObject(unsigned short index) const;
    /// detaches all objects attached to this node
    void DetachAllObjects();

    /// gets the orientation of the node as derived from all parents
    const core::Quaternion& _GetDerivedOrientation() const;
    /// gets the position of the node as derived from all parents
    const core::Vector3& _GetDerivedPosition() const;
    /// gets the scaling factor of the node as derived from all parents
    const core::Vector3& _GetDerivedScale() const;
    /// gets the full transformation matrix for this node
    /**
    @remarks
        This method returns the full transformation matrix
        for this node, including the effect of any parent node
        transformations, provided they have been updated using the Node::_update method.
        This should only be called by a SceneManager which knows the
        derived transforms have been updated before calling this method.
        Applications should just use the relative transforms.
    */
    const core::Matrix4& _GetFullTransform() const;

    /// internal method to update the Node
    /**
        @note
            Updates this scene node and any relevant children to incorporate transforms etc.
            Don't call this yourself unless you are writing a SceneManager implementation.
    */
    void _Update(bool parentHasChanged);

private:
    /// constructor, only to be called by the creator SceneManager
    SceneNode();
    /// only available internally - notification of parent.
    void SetParent(SceneNode* const p);

    /// pointer to parent node
    SceneNode* parent;
    /// collection of pointers to direct children
    ce_linked_list* children;

    /// stores the orientation of the node relative to it's parent.
    core::Quaternion orientation;
    /// stores the position/translation of the node relative to its parent.
    core::Vector3 position;
    /// stores the scaling factor applied to this node
    core::Vector3 scale;

    /** cached combined orientation.
        @par
            This member is the orientation derived by combining the
            local transformations and those of it's parents.
            This is updated when _updateFromParent is called by the
            SceneManager or the nodes parent.
    */
    mutable core::Quaternion derivedOrientation;

    /** cached combined position.
        @par
            This member is the position derived by combining the
            local transformations and those of it's parents.
            This is updated when _updateFromParent is called by the
            SceneManager or the nodes parent.
    */
    mutable core::Vector3 derivedPosition;

    /** cached combined scale.
        @par
            This member is the position derived by combining the
            local transformations and those of it's parents.
            This is updated when _updateFromParent is called by the
            SceneManager or the nodes parent.
    */
    mutable core::Vector3 derivedScale;

    /// cached derived transform as a 4x4 matrix
    mutable core::Matrix4 cachedTransform;
    mutable bool cachedTransformOutOfDate;
    bool updateChilds;

    mutable ce_dynamic_array objects;
};

} // namespace graphics
} // namespace chrissly
//------------------------------------------------------------------------------
#endif
