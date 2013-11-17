#ifndef VEHICLE_H_
#define VEHICLE_H_
//------------------------------------------------------------------------------
/**
    @class chrissly::physics::Vehicle

    14.02.2010 -> Creation

    (C) 2010 Christian Bleicher
*/
#include "rigidbody.h"
#include "wheel.h"

//------------------------------------------------------------------------------
namespace chrissly
{
namespace physics
{

class Vehicle : public RigidBody
{
public:
    /// construct from size, mass and wheelradius
    Vehicle(const chrissly::core::Vector3& halfSize, float mass, float wheelRadius);
    /// destructor
    ~Vehicle();
    /// apply steering angle to front wheels
    void SetSteering(float steering);
    /// apply transmission torque to back wheels
    void SetThrottle(float throttle, bool allWheel);
    /// apply brake torque apposing wheel velocity
    void SetBrakes(float brakes);
    /// apllying all forces on the rigidbody
    void Update(float duration);

private:
    /// default constructor
    Vehicle();
    /// wheels attached to the vehicle
    Wheel* wheels[4];
};

} // namespace physics
} // namespace chrissly
//------------------------------------------------------------------------------
#endif