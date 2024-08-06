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
    objects(NULL),
    scale(Vector3::UNIFORM_SCALE),
    derivedScale(Vector3::UNIFORM_SCALE),
    cachedTransform(Matrix4::IDENTITY),
    cachedTransformOutOfDate(true),
    updateChilds(false)
{
    ce_array_init(this->objects, 1U);
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
ce_linked_list*
SceneNode::GetAllChildren() const
{
    return this->children;
}

//------------------------------------------------------------------------------
/**
*/
void
SceneNode::RemoveChild(SceneNode* const node)
{
    ce_linked_list* it = this->children;
    while (it != NULL)
    {
        if ((SceneNode*)it->data == node)
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
SceneNode::RemoveAllChildren()
{
    ce_linked_list* it = this->children;
    while (it != NULL)
    {
        ((SceneNode*)it->data)->SetParent(NULL);
        ce_linked_list* node = it;
        it = it->next;
        ce_linked_list_remove(&this->children, node);
    }
}

//------------------------------------------------------------------------------
/**
*/
void
SceneNode::RemoveAndDestroyAllChildren()
{
    unsigned int i;
    for (i = 0U; i < ce_array_size(this->objects); ++i)
    {
        SceneManager::Instance()->DestroyEntity(this->objects[i]);
    }
    ce_array_delete(this->objects);

    ce_linked_list* it = this->children;
    while (it != NULL)
    {
        SceneNode* sceneNode = (SceneNode*)it->data;
        sceneNode->RemoveAndDestroyAllChildren();
        SceneManager::Instance()->DestroySceneNode(sceneNode);
        ce_linked_list* node = it;
        it = it->next;
        ce_linked_list_remove(&this->children, node);
    }
}

//------------------------------------------------------------------------------
/**
*/
void
SceneNode::SetOrientation(const Quaternion& q)
{
    this->orientation = q;
    this->updateChilds = this->cachedTransformOutOfDate = true;
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
    this->updateChilds = this->cachedTransformOutOfDate = true;
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
SceneNode::SetPosition(const Vector3& pos)
{
    this->position = pos;
    this->updateChilds = this->cachedTransformOutOfDate = true;
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
    this->updateChilds = this->cachedTransformOutOfDate = true;
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
    this->updateChilds = this->cachedTransformOutOfDate = true;
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
    this->updateChilds = this->cachedTransformOutOfDate = true;
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
    this->updateChilds = this->cachedTransformOutOfDate = true;
}

//------------------------------------------------------------------------------
/**
*/
void
SceneNode::SetVisible(bool vis)
{
    unsigned int i;
    for (i = 0U; i < ce_array_size(this->objects); ++i)
    {
        this->objects[i]->SetVisible(vis);
    }

    ce_linked_list* it = this->children;
    while (it != NULL)
    {
        ((SceneNode*)it->data)->SetVisible(vis);
        it = it->next;
    }
}

//------------------------------------------------------------------------------
/**
*/
void
SceneNode::AttachObject(Entity* const obj)
{
    ce_array_push_back(this->objects, obj);

    obj->_NotifyAttached(this);
}

//------------------------------------------------------------------------------
/**
*/
unsigned short
SceneNode::NumAttachedObjects() const
{
    return (unsigned short)ce_array_size(this->objects);
}

//------------------------------------------------------------------------------
/**
*/
Entity*
SceneNode::GetAttachedObject(unsigned short index) const
{
    return this->objects[index];
}
//------------------------------------------------------------------------------
/**
*/
void
SceneNode::DetachObject(Entity* const obj)
{
    obj->_NotifyAttached(NULL);
    ce_array_erase(this->objects, obj);
}

//------------------------------------------------------------------------------
/**
*/
void
SceneNode::DetachAllObjects()
{
    unsigned int i;
    for (i = 0U; i < ce_array_size(this->objects); ++i)
    {
        this->objects[i]->_NotifyAttached(NULL);
    }
    ce_array_delete(this->objects);
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
SceneNode::_Update(bool parentHasChanged)
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
        ((SceneNode*)it->data)->_Update(this->updateChilds);
        it = it->next;
    }

    this->updateChilds = false;
}

//------------------------------------------------------------------------------
/**
*/
void
SceneNode::SetParent(SceneNode* const p)
{
    this->parent = p;
    this->updateChilds = this->cachedTransformOutOfDate = true;
}

} // namespace graphics
} // namespace chrissly
