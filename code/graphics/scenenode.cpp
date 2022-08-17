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
    objects(NULL)
{
    ce_array_init(this->objects, 1U);
}

//------------------------------------------------------------------------------
/**
*/
SceneNode::~SceneNode()
{
    this->DetachAllObjects();
}

//------------------------------------------------------------------------------
/**
*/
SceneNode* const
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
SceneNode* const
SceneNode::GetParentSceneNode() const
{
    return (SceneNode*)this->parent;
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
Entity* const
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

} // namespace graphics
} // namespace chrissly
