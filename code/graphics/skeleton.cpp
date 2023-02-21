//------------------------------------------------------------------------------
//  skeleton.cpp
//  (C) 2022 Christian Bleicher
//------------------------------------------------------------------------------
#include "skeleton.h"
#include "chrisslyarray.h"

namespace chrissly
{
namespace graphics
{

using namespace chrissly::core;

//------------------------------------------------------------------------------
/**
*/
Skeleton::Skeleton(unsigned int numBones) :
    parentIndex(NULL),
    localMatrix(NULL),
    blendMode(ANIMBLEND_AVERAGE)
{
    ce_array_init(this->parentIndex, numBones);
    ce_array_init(this->localMatrix, numBones);
    ce_array_init(this->inverseModelMatrix, numBones);
}

//------------------------------------------------------------------------------
/**
*/
Skeleton::~Skeleton()
{
    ce_array_delete(this->parentIndex);
    ce_array_delete(this->localMatrix);
    ce_array_delete(this->inverseModelMatrix);
}

//------------------------------------------------------------------------------
/**
*/
void
Skeleton::SetBone(int index, int parent, Matrix4 const& local, Matrix4 const& invModel)
{
    this->parentIndex[index] = parent;
    ce_array_header(this->parentIndex)->size++;
    this->localMatrix[index] = local;
    ce_array_header(this->localMatrix)->size++;
    this->inverseModelMatrix[index] = invModel;
    ce_array_header(this->inverseModelMatrix)->size++;
}

//------------------------------------------------------------------------------
/**
*/
Matrix4* const
Skeleton::GetLocalTransformMatrices() const
{
    return this->localMatrix;
}

//------------------------------------------------------------------------------
/**
*/
Matrix4* const
Skeleton::GetInverseModelTransformMatrices() const
{
    return this->inverseModelMatrix;
}

//------------------------------------------------------------------------------
/**
*/
int* const
Skeleton::GetParentIndicies() const
{
    return this->parentIndex;
}

//------------------------------------------------------------------------------
/**
*/
void
Skeleton::SetBlendMode(SkeletonAnimationBlendMode mode)
{
    this->blendMode = mode;
}

//------------------------------------------------------------------------------
/**
*/
SkeletonAnimationBlendMode
Skeleton::GetBlendMode() const
{
    return this->blendMode;
}

} // namespace graphics
} // namespace chrissly
