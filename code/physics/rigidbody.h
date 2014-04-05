#ifndef RIGIDBODY_H_
#define RIGIDBODY_H_
//------------------------------------------------------------------------------
/**
    @class chrissly::physics::RigidBody

    (C) 2010 Christian Bleicher
*/
#include "vector3.h"

//------------------------------------------------------------------------------
namespace chrissly
{
namespace physics
{

class RigidBody
{
public:
    /// default constructor
    RigidBody();
    /// destructor
    virtual ~RigidBody();
    /// initialize size and mass
    void Setup(const chrissly::core::Vector3& halfSize, float mass);

    /// sets the position of the rigid body by component
    void SetPosition(float x, float y, float z);
    /// sets the position of the rigid body
    void SetPosition(const chrissly::core::Vector3& pos);
    /// gets the position of the rigid body
    const chrissly::core::Vector3& GetPosition() const;

    /// set angle of the rigid body
    void SetYaw(float angle);
    /// gets angle of the rigid body
    float GetYaw();

    /// adds the given force to the given point on the rigid body
    void AddForceAtPoint(const chrissly::core::Vector3& force, const chrissly::core::Vector3& point);
    /// euler-integrates the rigid body forward in time by the given amount
    void Integrate(float duration);
    /// sets velocity and angular velocity to zero
    void Reset();

    /// gets the velocity of a point on body
    const chrissly::core::Vector3 GetVelocityAtPoint(const chrissly::core::Vector3& point);
    /// take a world vector and make it a local vector
    const chrissly::core::Vector3 WorldToLocal(const chrissly::core::Vector3& world);
    /// take a local vector and make it a world vector
    const chrissly::core::Vector3 LocalToWorld(const chrissly::core::Vector3& local);

private:
    /// linear position of the rigid body in world space
    chrissly::core::Vector3 position;
    /// linear velocity of the rigid body in world space
    chrissly::core::Vector3 velocity;
    /// accumulated force to be applied at the next integration step
    chrissly::core::Vector3 forceAccum;
    /// mass of the rigid body
    float mass;
    /// angle of the rigid body
    float yaw;
    /// angular velocity of the rigid body
    float angularVelocity;
    /// torque (rotational force) accumulation of the rigid body
    float torque;
    /// inertia (rotational mass) of the rigid body
    float inertia;
};

} // namespace physics
} // namespace chrissly
//------------------------------------------------------------------------------
#endif