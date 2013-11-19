#ifndef WHEEL_H_
#define WHEEL_H_
//------------------------------------------------------------------------------
/**
    @class chrissly::physics::Wheel

    (C) 2010 Christian Bleicher
*/
#include "vector3.h"

//------------------------------------------------------------------------------
namespace chrissly
{
namespace physics
{

class Wheel
{
public:
    /// constructor with position and radius
    Wheel(const chrissly::core::Vector3& position, float radius);
    /// destructor
    ~Wheel();
    /// sets the steeringangle of the wheel
    void SetSteeringAngle(float newAngle);
    /// adds a force to the wheel
    void AddTransmissionTorque(float newValue);
    /// gets the speed of the wheel
    float GetWheelSpeed();
    /// gets the position of the wheel relative to the body
    const chrissly::core::Vector3& GetAttachPoint();
    /// calculates the force the wheel applies to the body
    chrissly::core::Vector3 CalculateForce(const chrissly::core::Vector3& relativeGroundSpeed, float timeStep);

private:
    /// default constructor
    Wheel();
    /// position of the wheel relative to the body
    chrissly::core::Vector3 position;
    /// radius of the wheel
    float wheelRadius;
    /// speed of the wheel
    float wheelSpeed;
    /// torque of the wheel
    float wheelTorque;
    /// inertia of the wheel
    float wheelInertia;
    /// forward axis
    chrissly::core::Vector3 forwardAxis;
    /// side axis
    chrissly::core::Vector3 sideAxis;
};

} // namespace physics
} // namespace chrissly
//------------------------------------------------------------------------------
#endif