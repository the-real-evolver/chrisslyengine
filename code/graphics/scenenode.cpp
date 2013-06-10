//------------------------------------------------------------------------------
//  scenenode.cpp
//  (C) 2011 Christian Bleicher
//------------------------------------------------------------------------------
#include "scenenode.h"
#include "scenemanager.h"
#include "debug.h"

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
    scale(Vector3(1.0f, 1.0f, 1.0f)),
    derivedScale(Vector3(1.0f, 1.0f, 1.0f)),
    cachedTransform(Matrix4::IDENTITY),
    cachedTransformOutOfDate(true),
    numAttachedObjects(0)
{
    DynamicArrayInit(&this->objectMap, 0);
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

        // Add altered position vector to parents
        this->derivedPosition += this->parent->_GetDerivedPosition();
    }
    else
    {
        // root node, no parent
        this->derivedOrientation = this->orientation;
        this->derivedPosition = this->position;
        this->derivedScale = this->scale;
    }

    LinkedList* it = this->children;
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

    linkedlistAdd(&this->children, sceneNode);

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
    LinkedList* it = this->children;
    while (it != NULL)
    {
        ((SceneNode*)it->data)->SetParent(NULL);
        LinkedList* node = it;
        it = it->next;
        linkedlistRemove(node);
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
SceneNode::SetScale(const Vector3& scale)
{
    this->scale = scale;
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
    this->Rotate(Vector3(0.0f, 0.0f, 1.0f), angle);
}

//------------------------------------------------------------------------------
/**
*/
void
SceneNode::Pitch(float angle)
{
    this->Rotate(Vector3(1.0f, 0.0f, 0.0f), angle);
}
//------------------------------------------------------------------------------
/**
*/
void
SceneNode::Yaw(float angle)
{
    this->Rotate(Vector3(0.0f, 1.0f, 0.0f), angle);

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
    // Normalise quaternion to avoid drift
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

        // Set up final matrix with scale, rotation and translation
        this->cachedTransform[0][0] = this->derivedScale.x * rot3x3[0][0];
        this->cachedTransform[0][1] = this->derivedScale.y * rot3x3[0][1];
        this->cachedTransform[0][2] = this->derivedScale.z * rot3x3[0][2];
        this->cachedTransform[0][3] = this->derivedPosition.x;
        this->cachedTransform[1][0] = this->derivedScale.x * rot3x3[1][0];
        this->cachedTransform[1][1] = this->derivedScale.y * rot3x3[1][1];
        this->cachedTransform[1][2] = this->derivedScale.z * rot3x3[1][2];
        this->cachedTransform[1][3] = this->derivedPosition.y;
        this->cachedTransform[2][0] = this->derivedScale.x * rot3x3[2][0];
        this->cachedTransform[2][1] = this->derivedScale.y * rot3x3[2][1];
        this->cachedTransform[2][2] = this->derivedScale.z * rot3x3[2][2];
        this->cachedTransform[2][3] = this->derivedPosition.z;

        // No projection term
        this->cachedTransform[3][0] = 0.0f;
        this->cachedTransform[3][1] = 0.0f;
        this->cachedTransform[3][2] = 0.0f;
        this->cachedTransform[3][3] = 1.0f;
        
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
    if (!DynamicArraySet(&this->objectMap, this->numAttachedObjects, obj))
    {
        CE_ASSERT(false, "SceneNode::AttachObject(): can't add Entity to objectmap");
    }

    obj->_NotifyAttached(this);

    this->numAttachedObjects++;
}

//------------------------------------------------------------------------------
/**
*/
unsigned short
SceneNode::NumAttachedObjects() const
{
    return this->numAttachedObjects;
}

//------------------------------------------------------------------------------
/**
*/
Entity*
SceneNode::GetAttachedObject(unsigned short index) const
{
    return (Entity*)DynamicArrayGet(&this->objectMap, index);
}

//------------------------------------------------------------------------------
/**
*/
void
SceneNode::DetachAllObjects()
{
    DynamicArrayDelete(&this->objectMap);
    this->numAttachedObjects = 0;
}

//------------------------------------------------------------------------------
/**
*/
void
SceneNode::SetParent(SceneNode* parent)
{
    this->parent = parent;
    this->cachedTransformOutOfDate = true;
}

} // namespace graphics
} // namespace chrissly
