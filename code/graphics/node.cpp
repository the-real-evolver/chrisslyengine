//------------------------------------------------------------------------------
//  node.cpp
//  (C) 2022 Christian Bleicher
//------------------------------------------------------------------------------
#include "node.h"

namespace chrissly
{
namespace graphics
{

using namespace chrissly::core;

//------------------------------------------------------------------------------
/**
*/
Node::Node() :
    parent(NULL),
    children(NULL),
    scale(Vector3::UNIFORM_SCALE),
    derivedScale(Vector3::UNIFORM_SCALE),
    cachedTransform(Matrix4::IDENTITY),
    cachedTransformOutOfDate(true),
    updateChilds(false)
{

}

//------------------------------------------------------------------------------
/**
*/
Node::~Node()
{
    this->RemoveAllChildren();
}

//------------------------------------------------------------------------------
/**
*/
Node* const
Node::GetParentNode() const
{
    return this->parent;
}

//------------------------------------------------------------------------------
/**
*/
ce_linked_list* const
Node::GetAllChildren() const
{
    return this->children;
}

//------------------------------------------------------------------------------
/**
*/
void
Node::RemoveChild(Node* const node)
{
    ce_linked_list* it = this->children;
    while (it != NULL)
    {
        if ((Node*)it->data == node)
        {
            ce_linked_list_remove(&this->children, it);
            break;
        }
        it = it->next;
    }
}

//------------------------------------------------------------------------------
/**
*/
void
Node::RemoveAllChildren()
{
    ce_linked_list* it = this->children;
    while (it != NULL)
    {
        ((Node*)it->data)->SetParent(NULL);
        ce_linked_list* node = it;
        it = it->next;
        ce_linked_list_remove(&this->children, node);
    }
}

//------------------------------------------------------------------------------
/**
*/
const Quaternion&
Node::GetOrientation() const
{
    return this->orientation;
}

//------------------------------------------------------------------------------
/**
*/
void
Node::SetOrientation(const Quaternion& q)
{
    this->orientation = q;
    this->updateChilds = this->cachedTransformOutOfDate = true;
}

//------------------------------------------------------------------------------
/**
*/
void
Node::SetOrientation(float w, float x, float y, float z)
{
    this->orientation.w = w;
    this->orientation.x = x;
    this->orientation.y = y;
    this->orientation.z = z;
    this->updateChilds = this->cachedTransformOutOfDate = true;
}

//------------------------------------------------------------------------------
/**
*/
void
Node::SetPosition(const Vector3& pos)
{
    this->position = pos;
    this->updateChilds = this->cachedTransformOutOfDate = true;
}

//------------------------------------------------------------------------------
/**
*/
void
Node::SetPosition(float x, float y, float z)
{
    this->position.x = x;
    this->position.y = y;
    this->position.z = z;
    this->updateChilds = this->cachedTransformOutOfDate = true;
}

//------------------------------------------------------------------------------
/**
*/
const Vector3&
Node::GetPosition() const
{
    return this->position;
}

//------------------------------------------------------------------------------
/**
*/
void
Node::SetScale(const Vector3& s)
{
    this->scale = s;
    this->updateChilds = this->cachedTransformOutOfDate = true;
}

//------------------------------------------------------------------------------
/**
*/
void
Node::SetScale(float x, float y, float z)
{
    this->scale.x = x;
    this->scale.y = y;
    this->scale.z = z;
    this->updateChilds = this->cachedTransformOutOfDate = true;
}

//------------------------------------------------------------------------------
/**
*/
const Vector3&
Node::GetScale() const
{
    return this->scale;
}

//------------------------------------------------------------------------------
/**
*/
void
Node::Roll(float angle)
{
    this->Rotate(Vector3::UNIT_POSITIVE_Z, angle);
}

//------------------------------------------------------------------------------
/**
*/
void
Node::Pitch(float angle)
{
    this->Rotate(Vector3::UNIT_POSITIVE_X, angle);
}

//------------------------------------------------------------------------------
/**
*/
void
Node::Yaw(float angle)
{
    this->Rotate(Vector3::UNIT_POSITIVE_Y, angle);
}

//------------------------------------------------------------------------------
/**
*/
void
Node::Rotate(const Vector3& axis, float angle)
{
    Quaternion q;
    q.FromAngleAxis(angle, axis);
    this->Rotate(q);
}

//------------------------------------------------------------------------------
/**
*/
void
Node::Rotate(const Quaternion& q)
{
    // normalise quaternion to avoid drift
    Quaternion qnorm = q;
    qnorm.Normalise();
    this->orientation = this->orientation * qnorm;
    this->updateChilds = this->cachedTransformOutOfDate = true;
}

//------------------------------------------------------------------------------
/**
*/
const Quaternion&
Node::_GetDerivedOrientation() const
{
    return this->derivedOrientation;
}

//------------------------------------------------------------------------------
/**
*/
const Vector3&
Node::_GetDerivedPosition() const
{
    return this->derivedPosition;
}

//------------------------------------------------------------------------------
/**
*/
const Vector3&
Node::_GetDerivedScale() const
{
    return this->derivedScale;
}

//------------------------------------------------------------------------------
/**
*/
const Matrix4&
Node::_GetFullTransform() const
{
    if (this->cachedTransformOutOfDate)
    {
        // Ordering:
        //    1. Scale
        //    2. Rotate
        //    3. Translate

        Matrix3 rot3x3;
        this->derivedOrientation.ToRotationMatrix(rot3x3);

        // set up final matrix with scale, rotation and translation
        this->cachedTransform[0U][0U] = this->derivedScale.x * rot3x3[0U][0U];
        this->cachedTransform[0U][1U] = this->derivedScale.y * rot3x3[0U][1U];
        this->cachedTransform[0U][2U] = this->derivedScale.z * rot3x3[0U][2U];
        this->cachedTransform[0U][3U] = this->derivedPosition.x;
        this->cachedTransform[1U][0U] = this->derivedScale.x * rot3x3[1U][0U];
        this->cachedTransform[1U][1U] = this->derivedScale.y * rot3x3[1U][1U];
        this->cachedTransform[1U][2U] = this->derivedScale.z * rot3x3[1U][2U];
        this->cachedTransform[1U][3U] = this->derivedPosition.y;
        this->cachedTransform[2U][0U] = this->derivedScale.x * rot3x3[2U][0U];
        this->cachedTransform[2U][1U] = this->derivedScale.y * rot3x3[2U][1U];
        this->cachedTransform[2U][2U] = this->derivedScale.z * rot3x3[2U][2U];
        this->cachedTransform[2U][3U] = this->derivedPosition.z;

        // no projection term
        this->cachedTransform[3U][0U] = 0.0f;
        this->cachedTransform[3U][1U] = 0.0f;
        this->cachedTransform[3U][2U] = 0.0f;
        this->cachedTransform[3U][3U] = 1.0f;

        this->cachedTransformOutOfDate = false;
    }

    return this->cachedTransform;
}

//------------------------------------------------------------------------------
/**
*/
void
Node::_Update(bool parentHasChanged)
{
    if (this->parent)
    {
        Quaternion parentOrientation = this->parent->_GetDerivedOrientation();
        Vector3 parentScale = this->parent->_GetDerivedScale();

        // combine orientation with that of parent
        this->derivedOrientation = parentOrientation * this->orientation;

        // update scale
        this->derivedScale = parentScale * this->scale;

        // change position vector based on parent's orientation & scale
        this->derivedPosition = parentOrientation * (parentScale * this->position);

        // add altered position vector to parents
        this->derivedPosition += this->parent->_GetDerivedPosition();
    }
    else
    {
        // root node, no parent
        this->derivedOrientation = this->orientation;
        this->derivedPosition = this->position;
        this->derivedScale = this->scale;
    }

    if (parentHasChanged)
    {
        this->updateChilds = this->cachedTransformOutOfDate = true;
    }

    ce_linked_list* it = this->children;
    while (it != NULL)
    {
        ((Node*)it->data)->_Update(this->updateChilds);
        it = it->next;
    }

    this->updateChilds = false;
}

//------------------------------------------------------------------------------
/**
*/
void
Node::SetParent(Node* const p)
{
    this->parent = p;
    this->updateChilds = this->cachedTransformOutOfDate = true;
}

} // namespace graphics
} // namespace chrissly
