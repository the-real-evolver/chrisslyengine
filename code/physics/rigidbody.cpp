//------------------------------------------------------------------------------
//  rigidbody.cpp
//  (C) 2010 Christian Bleicher
//------------------------------------------------------------------------------
#include "rigidbody.h"
#include "chrisslymath.h"

namespace chrissly
{
namespace physics
{

using namespace chrissly::core;

//------------------------------------------------------------------------------
/**
*/
RigidBody::RigidBody() :
    position(Vector3()),
    velocity(Vector3()),
    forceAccum(Vector3()),
    mass(0.0f),
    yaw(0.0f),
    angularVelocity(0.0f),
    torque(0.0f),
    inertia(0.0f)
{

}

//------------------------------------------------------------------------------
/**
*/
RigidBody::~RigidBody()
{

}

//------------------------------------------------------------------------------
/**
*/
void
RigidBody::Setup(const Vector3& halfSize, float mass)
{
    this->mass = mass;

    // calculate cuboid inertia (rotation axis is y): inertia = 1/12 * mass * (x² + z²)
    this->inertia = (1.0f / 12.0f) * mass * (halfSize.x * halfSize.x + halfSize.z * halfSize.z);
}

//------------------------------------------------------------------------------
/**
*/
void
RigidBody::SetPosition(float x, float y, float z)
{
    this->position.x = x;
    this->position.y = y;
    this->position.z = z;
}

//------------------------------------------------------------------------------
/**
*/
void
RigidBody::SetPosition(const Vector3& pos)
{
    this->position = pos;
}

//------------------------------------------------------------------------------
/**
*/
const Vector3&
RigidBody::GetPosition() const
{
    return this->position;
}

//------------------------------------------------------------------------------
/**
*/
void
RigidBody::SetYaw(float angle)
{
    this->yaw = angle;
}
//------------------------------------------------------------------------------
/**
*/
float
RigidBody::GetYaw()
{
    return this->yaw;
}

//------------------------------------------------------------------------------
/**
*/
void
RigidBody::AddForceAtPoint(const Vector3& force, const Vector3& point)
{
    // add linear force
    this->forceAccum.x += force.x;
    this->forceAccum.z += force.z;

    // add torque (crossproduct point X force)
    this->torque += point.x * force.z - point.z * force.x;
}

//------------------------------------------------------------------------------
/**
*/
void
RigidBody::Integrate(float duration)
{
    // linear
    Vector3 acceleration;
    acceleration.x = this->forceAccum.x / this->mass;
    acceleration.z = this->forceAccum.z / this->mass;

    this->velocity.x += acceleration.x * duration; // should be duration * duration * 0.5f
    this->velocity.z += acceleration.z * duration;

    this->position.x += this->velocity.x * duration;
    this->position.z += this->velocity.z * duration;

    this->forceAccum.x = 0.0f;
    this->forceAccum.z = 0.0f;

    // angular
    float angularAcceleration = this->torque / this->inertia;
    this->angularVelocity += angularAcceleration * duration;
    this->yaw += this->angularVelocity * duration;
    this->torque = 0.0f;
}

//------------------------------------------------------------------------------
/**
*/
void
RigidBody::Reset()
{
    this->velocity.x = 0.0f;
    this->velocity.y = 0.0f;
    this->velocity.z = 0.0f;

    this->angularVelocity = 0.0f;
}

//------------------------------------------------------------------------------
/**
*/
const Vector3
RigidBody::GetVelocityAtPoint(const Vector3& point)
{
    Vector3 tangent;
    tangent.x = -point.z * this->angularVelocity + this->velocity.x;
    tangent.z = point.x * this->angularVelocity + this->velocity.z;

    return tangent;
}

//------------------------------------------------------------------------------
/**
*/
const Vector3
RigidBody::WorldToLocal(const Vector3& world)
{
    float sine   = Math::Sin(-this->yaw);
    float cosine = Math::Cos(-this->yaw);

    Vector3 local;
    local.x = world.x * cosine - world.z * sine;
    local.z = world.x * sine   + world.z * cosine;

    return local;
}

//------------------------------------------------------------------------------
/**
*/
const Vector3
RigidBody::LocalToWorld(const Vector3& relative)
{
    float sine   = Math::Sin(this->yaw);
    float cosine = Math::Cos(this->yaw);

    Vector3 world;
    world.x = relative.x * cosine - relative.z * sine;
    world.z = relative.x * sine   + relative.z * cosine;

    return world;
}

} // namespace physics
} // namespace chrissly
