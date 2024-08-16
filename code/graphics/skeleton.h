#ifndef SKELETON_H_
#define SKELETON_H_
//------------------------------------------------------------------------------
/**
    @class chrissly::graphics::Skeleton

    The bone hierarchy tree is flattened into multiple arrays. This means that
    the parent index of a bone always has to be lower than its own index.

    Datalayout: A "Bone" consists basically of a 4x4 transform matrix and an
    index to its parent bone. A Matrix4 is 64 bytes wich is the common size of a
    cacheline. They should be stored next to each other in memory. For this
    reason they are not packed in a struct together with the parent index.
    Instead a SOA (struct of arrays) approach is used where every component of
    a bone is just an array and the index is the bone id.

    (C) 2022 Christian Bleicher
*/
#include "matrix4.h"

//------------------------------------------------------------------------------
namespace chrissly
{
namespace graphics
{

/// defines how the blend weight is calculated when blending multiple animations
enum SkeletonAnimationBlendMode
{
    /// animations are applied by calculating a weighted average of all animations
    ANIMBLEND_AVERAGE,
    /// animations are applied by calculating a cumulative total
    ANIMBLEND_ADDITIVE
};

class Skeleton
{
public:
    /// constructor with the number of bones
    Skeleton(unsigned int numBones);
    /// destructor
    ~Skeleton();
    /// set bone parameters
    void SetBone(int index, int parent, chrissly::core::Matrix4 const& local, chrissly::core::Matrix4 const& invModel);
    /// gets the list of local transform matrices
    chrissly::core::Matrix4* GetLocalTransformMatrices() const;
    /// gets the list of inverse model transform matrices
    chrissly::core::Matrix4* GetInverseModelTransformMatrices() const;
    /// gets the list of parent indicies
    int* GetParentIndicies() const;
    /// gets the animation blending mode which this skeleton will use
    SkeletonAnimationBlendMode GetBlendMode() const;
    /// sets the animation blending mode this skeleton will use
    void SetBlendMode(SkeletonAnimationBlendMode state);

private:
    /// index of the parent bone
    int* parentIndex;
    /// transformation relative to parent bone
    chrissly::core::Matrix4* localMatrix;
    /// inverse transformation relative to skeleton
    chrissly::core::Matrix4* inverseModelMatrix;
    /// the animation blending mode this skeleton will use
    SkeletonAnimationBlendMode blendMode;
};

} // namespace graphics
} // namespace chrissly
//------------------------------------------------------------------------------
#endif
