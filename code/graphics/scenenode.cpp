//------------------------------------------------------------------------------
//  scenenode.cpp
//  (C) 2011 Christian Bleicher
//------------------------------------------------------------------------------
#include "scenenode.h"
#include "scenemanager.h"

namespace chrissly
{
namespace graphics
{

using namespace chrissly::core;

//------------------------------------------------------------------------------
/**
*/
SceneNode::SceneNode() :
    parent(NULL),
    children(NULL),
    scale(Vector3::UNIFORM_SCALE),
    derivedScale(Vector3::UNIFORM_SCALE),
    cachedTransform(Matrix4::IDENTITY),
    cachedTransformOutOfDate(true)
{
    ce_dynamic_array_init(&this->objects, 1U);
}

//------------------------------------------------------------------------------
/**
*/
SceneNode::~SceneNode()
{
    this->RemoveAllChildren();
    this->DetachAllObjects();
}

//------------------------------------------------------------------------------
/**
*/
void
SceneNode::_Update()
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

    ce_linked_list* it = this->children;
    while (it != NULL)
    {
        ((SceneNode*)it->data)->_Update();
        it = it->next;
    }
}

//------------------------------------------------------------------------------
/**
*/
SceneNode*
SceneNode::CreateChildSceneNode()
{
    SceneNode* sceneNode = SceneManager::Instance()->CreateSceneNode();
    sceneNode->SetParent(this);

    ce_linked_list_add(&this->children, sceneNode);

    return sceneNode;
}

//------------------------------------------------------------------------------
/**
*/
SceneNode*
SceneNode::GetParentSceneNode() const
{
    return this->parent;
}

//------------------------------------------------------------------------------
/**
*/
void
SceneNode::RemoveAllChildren()
{
    ce_linked_list* it = this->children;
    while (it != NULL)
    {
        ((SceneNode*)it->data)->SetParent(NULL);
        ce_linked_list* node = it;
        it = it->next;
        ce_linked_list_remove(node);
    }
    this->children = NULL;
}

//------------------------------------------------------------------------------
/**
*/
const Quaternion&
SceneNode::GetOrientation() const
{
    return this->orientation;
}

//------------------------------------------------------------------------------
/**
*/
void
SceneNode::SetOrientation(const Quaternion& q)
{
    this->orientation = q;
    this->cachedTransformOutOfDate = true;
}

//------------------------------------------------------------------------------
/**
*/
void
SceneNode::SetOrientation(float w, float x, float y, float z)
{
    this->orientation.w = w;
    this->orientation.x = x;
    this->orientation.y = y;
    this->orientation.z = z;
    this->cachedTransformOutOfDate = true;
}

//------------------------------------------------------------------------------
/**
*/
void
SceneNode::SetPosition(const Vector3& pos)
{
    this->position = pos;
    this->cachedTransformOutOfDate = true;
}

//------------------------------------------------------------------------------
/**
*/
void
SceneNode::SetPosition(float x, float y, float z)
{
    this->position.x = x;
    this->position.y = y;
    this->position.z = z;
    this->cachedTransformOutOfDate = true;
}

//------------------------------------------------------------------------------
/**
*/
const Vector3&
SceneNode::GetPosition() const
{
    return this->position;
}

//------------------------------------------------------------------------------
/**
*/
void
SceneNode::SetScale(const Vector3& s)
{
    this->scale = s;
    this->cachedTransformOutOfDate = true;
}

//------------------------------------------------------------------------------
/**
*/
void
SceneNode::SetScale(float x, float y, float z)
{
    this->scale.x = x;
    this->scale.y = y;
    this->scale.z = z;
    this->cachedTransformOutOfDate = true;
}

//------------------------------------------------------------------------------
/**
*/
const Vector3&
SceneNode::GetScale() const
{
    return this->scale;
}

//------------------------------------------------------------------------------
/**
*/
const Quaternion&
SceneNode::_GetDerivedOrientation() const
{
    return this->derivedOrientation;
}

//------------------------------------------------------------------------------
/**
*/
const Vector3&
SceneNode::_GetDerivedPosition() const
{
    return this->derivedPosition;
}

//------------------------------------------------------------------------------
/**
*/
const Vector3&
SceneNode::_GetDerivedScale() const
{
    return this->derivedScale;
}

//------------------------------------------------------------------------------
/**
*/
void
SceneNode::Roll(float angle)
{
    this->Rotate(Vector3::UNIT_POSITIVE_Z, angle);
}

//------------------------------------------------------------------------------
/**
*/
void
SceneNode::Pitch(float angle)
{
    this->Rotate(Vector3::UNIT_POSITIVE_X, angle);
}

//------------------------------------------------------------------------------
/**
*/
void
SceneNode::Yaw(float angle)
{
    this->Rotate(Vector3::UNIT_POSITIVE_Y, angle);
}

//------------------------------------------------------------------------------
/**
*/
void
SceneNode::Rotate(const Vector3& axis, float angle)
{
    Quaternion q;
    q.FromAngleAxis(angle, axis);
    this->Rotate(q);
}

//------------------------------------------------------------------------------
/**
*/
void
SceneNode::Rotate(const Quaternion& q)
{
    // normalise quaternion to avoid drift
    Quaternion qnorm = q;
    qnorm.Normalise();
    this->orientation = this->orientation * qnorm;
    this->cachedTransformOutOfDate = true;
}

//------------------------------------------------------------------------------
/**
*/
const Matrix4&
SceneNode::_GetFullTransform() const
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
SceneNode::AttachObject(Entity* obj)
{
    ce_dynamic_array_push_back(&this->objects, obj);

    obj->_NotifyAttached(this);
}

//------------------------------------------------------------------------------
/**
*/
unsigned short
SceneNode::NumAttachedObjects() const
{
    return (unsigned short)this->objects.size;
}

//------------------------------------------------------------------------------
/**
*/
Entity*
SceneNode::GetAttachedObject(unsigned short index) const
{
    return (Entity*)ce_dynamic_array_get(&this->objects, index);
}

//------------------------------------------------------------------------------
/**
*/
void
SceneNode::DetachAllObjects()
{
    ce_dynamic_array_delete(&this->objects);
}

//------------------------------------------------------------------------------
/**
*/
void
SceneNode::SetParent(SceneNode* p)
{
    this->parent = p;
    this->cachedTransformOutOfDate = true;
}

} // namespace graphics
} // namespace chrissly
