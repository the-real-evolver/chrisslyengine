#ifndef NODE_H_
#define NODE_H_
//------------------------------------------------------------------------------
/**
    @class chrissly::graphics::Node

    (C) 2022 Christian Bleicher
*/
#include "linkedlist.h"
#include "quaternion.h"
#include "vector3.h"
#include "matrix4.h"

//------------------------------------------------------------------------------
namespace chrissly
{
namespace graphics
{

class Node
{
public:
    /// destructor
    ~Node();

    /// gets the parent of this Node
    Node* const GetParentNode() const;
    /// gets list of all child nodes
    ce_linked_list* const GetAllChildren() const;
    /// removes/detaches the specified child from this node
    void RemoveChild(Node* const node);
    /// removes all child nodes attached to this node
    /**
        Does not delete the nodes, just detaches them from
        this parent, potentially to be reattached elsewhere.
    */
    void RemoveAllChildren();

    /// sets the orientation of this node via a quaternion
    /**
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
        @par
            Note that like rotations, scalings are oriented around the node's origin.
    */
    void SetScale(const core::Vector3& s);
    /// sets the scaling factor applied to this node
    void SetScale(float x, float y, float z);
    /// gets the scaling factor of this node
    const core::Vector3& GetScale() const;
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
            transformations, provided they have been updated using the Node::_Update method.
    */
    const core::Matrix4& _GetFullTransform() const;

    /// internal method to update the Node
    /**
        @note
            Updates this scene node and any relevant children to incorporate transforms etc.
    */
    void _Update(bool parentHasChanged);

protected:
    /// constructor, only to be called by the creator
    Node();
    /// only available internally - notification of parent
    void SetParent(Node* const p);

    /// pointer to parent node
    Node* parent;
    /// collection of pointers to direct children
    ce_linked_list* children;

    /// stores the orientation of the node relative to it's parent
    core::Quaternion orientation;
    /// stores the position/translation of the node relative to its parent
    core::Vector3 position;
    /// stores the scaling factor applied to this node
    core::Vector3 scale;

    /// cached combined orientation
    /**
        @par
            This member is the orientation derived by combining the
            local transformations and those of it's parents.
    */
    mutable core::Quaternion derivedOrientation;

    /// cached combined position
    /**
        @par
            This member is the position derived by combining the
            local transformations and those of it's parents.
    */
    mutable core::Vector3 derivedPosition;

    /// cached combined scale
    /**
        @par
            This member is the position derived by combining the
            local transformations and those of it's parents.
    */
    mutable core::Vector3 derivedScale;

    /// cached derived transform as a 4x4 matrix
    mutable core::Matrix4 cachedTransform;
    mutable bool cachedTransformOutOfDate;
    bool updateChilds;
};

} // namespace graphics
} // namespace chrissly
//------------------------------------------------------------------------------
#endif
