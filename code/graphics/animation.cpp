//------------------------------------------------------------------------------
//  animation.cpp
//  (C) 2012 Christian Bleicher
//------------------------------------------------------------------------------
#include "animation.h"
#include "entity.h"
#include "chrisslyarray.h"
#include "chrisslymath.h"
#include "memoryallocatorconfig.h"

namespace chrissly
{
namespace graphics
{

using namespace chrissly::core;

//------------------------------------------------------------------------------
/**
*/
Animation::Animation(const char* const name, float length) :
    length(length),
    vertexTracks(NULL),
    boneTracks(NULL)
{
    this->name = name;
    ce_array_init(this->vertexTracks, 1U);
}

//------------------------------------------------------------------------------
/**
*/
Animation::~Animation()
{
    this->DestroyAllVertexTracks();
    this->DestroyAllBoneTracks();
}

//------------------------------------------------------------------------------
/**
*/
const char* const
Animation::GetName() const
{
    return this->name.C_Str();
}

//------------------------------------------------------------------------------
/**
*/
float
Animation::GetLength() const
{
    return this->length;
}

//------------------------------------------------------------------------------
/**
*/
VertexAnimationTrack* const
Animation::CreateVertexTrack(unsigned char handle)
{
    VertexAnimationTrack* vertexAnimationTrack = CE_NEW VertexAnimationTrack(handle);
    ce_array_push_back(this->vertexTracks, vertexAnimationTrack);

    return vertexAnimationTrack;
}

//------------------------------------------------------------------------------
/**
*/
unsigned short
Animation::GetNumVertexTracks() const
{
    return (unsigned short)ce_array_size(this->vertexTracks);
}

//------------------------------------------------------------------------------
/**
*/
VertexAnimationTrack* const
Animation::GetVertexTrack(unsigned short index) const
{
    return this->vertexTracks[index];
}

//------------------------------------------------------------------------------
/**
*/
void
Animation::DestroyAllVertexTracks()
{
    unsigned int i;
    for (i = 0U; i < ce_array_size(this->vertexTracks); ++i)
    {
        CE_DELETE this->vertexTracks[i];
    }
    ce_array_delete(this->vertexTracks);
}

//------------------------------------------------------------------------------
/**
*/
BoneAnimationTrack* const
Animation::CreateBoneTrack(unsigned int numKeyFrames)
{
    BoneAnimationTrack* boneAnimationTrack = CE_NEW BoneAnimationTrack(numKeyFrames);
    ce_array_push_back(this->boneTracks, boneAnimationTrack);

    return boneAnimationTrack;
}

//------------------------------------------------------------------------------
/**
*/
unsigned short
Animation::GetNumBoneTracks() const
{
    return (unsigned short)ce_array_size(this->boneTracks);
}

//------------------------------------------------------------------------------
/**
*/
BoneAnimationTrack* const
Animation::GetBoneTrack(unsigned short index) const
{
    return this->boneTracks[index];
}

//------------------------------------------------------------------------------
/**
*/
void
Animation::DestroyAllBoneTracks()
{
    unsigned int i;
    for (i = 0U; i < ce_array_size(this->boneTracks); ++i)
    {
        CE_DELETE this->boneTracks[i];
    }
    ce_array_delete(this->boneTracks);
}

//------------------------------------------------------------------------------
/**
*/
void
Animation::Apply(Entity* const entity, float timePos, float blendWeight)
{
    if (timePos > this->length && this->length > 0.0f)
    {
        timePos = Math::Fmod(timePos, this->length);
    }

    // update morph animation
    unsigned int i;
    for (i = 0U; i < ce_array_size(this->vertexTracks); ++i)
    {
        VertexAnimationTrack* vertexAnimTrack = this->vertexTracks[i];
        unsigned short numKeys = vertexAnimTrack->GetNumKeyFrames() - 1U;
        unsigned short keyIndex;
        for (keyIndex = 0U; keyIndex < numKeys; ++keyIndex)
        {
            VertexMorphKeyFrame* key = vertexAnimTrack->GetVertexMorphKeyFrame(keyIndex);
            VertexMorphKeyFrame* nextKey = vertexAnimTrack->GetVertexMorphKeyFrame(keyIndex + 1U);
            if (timePos >= key->GetTime() && timePos < nextKey->GetTime())
            {
                SubEntity* subEntity = entity->GetSubEntity(vertexAnimTrack->GetHandle());
                subEntity->SetMorphWeight((timePos - key->GetTime()) / (nextKey->GetTime() - key->GetTime()));
                if (subEntity->GetCurrentTimeIndex() != keyIndex)
                {
                    vertexAnimTrack->ApplyToVertexData(subEntity->GetMorphVertexData(), keyIndex);
                    subEntity->SetCurrentTimeIndex(keyIndex);
                }
                break;
            }
        }
    }

    // update skeletal animation
    Skeleton* skeleton = entity->GetMesh()->GetSkeleton();
    if (skeleton != NULL && ce_array_size(this->boneTracks) > 0U)
    {
        static Matrix4 localTransform[32U] = {};
        static Matrix4 modelTransform[32U] = {};

        // calc current keyframe and t
        float t = 0.0f;
        unsigned int currentKeyframe = 0U;
        float* times = this->boneTracks[0U]->GetTimeIndicies();
        unsigned int numKeys = ce_array_size(times) - 1U;
        for (i = 0U; i < numKeys; ++i)
        {
            if (timePos >= times[i] && timePos < times[i + 1U])
            {
                currentKeyframe = i;
                t = (timePos - times[i]) / (times[i + 1U] - times[i]);
                break;
            }
        }

        int* parentIndex = skeleton->GetParentIndicies();
        Matrix4* boneLocalMatrix = skeleton->GetLocalTransformMatrices();
        Matrix4* bonesInverseModelMatrix = skeleton->GetInverseModelTransformMatrices();
        unsigned int numBones = ce_array_size(boneLocalMatrix);
        Matrix4* boneMatrix = entity->GetBoneMatrices();
        Matrix4* blendMatrix = entity->GetBlendMatrices();

        // 1. restpose * animation keyframe local matrix
        for (i = 0U; i < numBones; ++i)
        {
            Matrix4* animKeyMatrix = this->boneTracks[i]->GetTransformMatrices();

            // interpolate keyframe and weight blend with keyframes from other active animations
            Vector3 p(animKeyMatrix[currentKeyframe][0U][3U], animKeyMatrix[currentKeyframe][1U][3U], animKeyMatrix[currentKeyframe][2U][3U]);
            Vector3 pn(animKeyMatrix[currentKeyframe + 1U][0U][3U], animKeyMatrix[currentKeyframe + 1U][1U][3U], animKeyMatrix[currentKeyframe + 1U][2U][3U]);
            Vector3 pos = Vector3(blendMatrix[i][0U][3U], blendMatrix[i][1U][3U], blendMatrix[i][2U][3U]) + Vector3::Lerp(p, pn, t) * blendWeight;

            Quaternion q, qn, qBlend;
            q.FromRotationMatrix(animKeyMatrix[currentKeyframe].To3x3());
            qn.FromRotationMatrix(animKeyMatrix[currentKeyframe + 1U].To3x3());
            qBlend.FromRotationMatrix(blendMatrix[i].To3x3());
            Matrix3 rm;
            (qBlend * Quaternion::Nlerp(Quaternion(), Quaternion::Nlerp(q, qn, t), blendWeight)).ToRotationMatrix(rm);

            blendMatrix[i] = Matrix4(rm[0U][0U], rm[0U][1U], rm[0U][2U], pos.x,
                                     rm[1U][0U], rm[1U][1U], rm[1U][2U], pos.y,
                                     rm[2U][0U], rm[2U][1U], rm[2U][2U], pos.z,
                                     0.0f,       0.0f,       0.0f,       1.0f);

            localTransform[i] = boneLocalMatrix[i] * blendMatrix[i];
        }

        // 2. propagate animation transform from rootbone to children
        modelTransform[0U] = localTransform[0U];
        for (i = 1U; i < numBones; ++i)
        {
            modelTransform[i] = modelTransform[parentIndex[i]] * localTransform[i];
        }

        // 3. model matrix * inverse bone matrix
        for (i = 0U; i < numBones; ++i)
        {
            boneMatrix[i] = modelTransform[i] * bonesInverseModelMatrix[i];
        }
    }
}

} // namespace graphics
} // namespace chrissly
