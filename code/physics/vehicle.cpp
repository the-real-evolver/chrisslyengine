//------------------------------------------------------------------------------
//  vehicle.cpp
//  (C) 2010 Christian Bleicher
//------------------------------------------------------------------------------
#include "vehicle.h"
#include "memoryallocatorconfig.h"

namespace chrissly
{
namespace physics
{

using namespace chrissly::core;

//------------------------------------------------------------------------------
/**
*/
Vehicle::Vehicle()
{

}

//------------------------------------------------------------------------------
/**
*/
Vehicle::Vehicle(const Vector3& halfSize, float mass, float wheelRadius)
{
    Vector3 position;

    // front wheels
    position.x = halfSize.x;
    position.z = halfSize.z;
    this->wheels[0] = CE_NEW Wheel(position, wheelRadius);
    position.x = -halfSize.x;
    position.z = halfSize.z;
    this->wheels[1] = CE_NEW Wheel(position, wheelRadius);

    // rear wheels
    position.x = halfSize.x;
    position.z = -halfSize.z;
    this->wheels[2] = CE_NEW Wheel(position, wheelRadius);
    position.x = -halfSize.x;
    position.z = -halfSize.z;
    this->wheels[3] = CE_NEW Wheel(position, wheelRadius);

    RigidBody::Setup(halfSize, mass);
}

//------------------------------------------------------------------------------
/**
*/
Vehicle::~Vehicle()
{
    unsigned int i;
    for (i = 0; i < 4; ++i)
    {
        CE_DELETE this->wheels[i];
    }
}

//------------------------------------------------------------------------------
/**
*/
void
Vehicle::SetSteering(float steering)
{
    const float steeringLock = 0.75f;
    const float steeringAngle = -steering * steeringLock;

    // apply steering angle to front wheels
    this->wheels[0]->SetSteeringAngle(steeringAngle);
    this->wheels[1]->SetSteeringAngle(steeringAngle);
}

//------------------------------------------------------------------------------
/**
*/
void
Vehicle::SetThrottle(float throttle, bool allWheel)
{
    const float torque = 20.0f;
    const float transmissionTorque = throttle * torque;

    // apply transmission torque to back wheels
    if (allWheel)
    {
        this->wheels[0]->AddTransmissionTorque(transmissionTorque);
        this->wheels[1]->AddTransmissionTorque(transmissionTorque);
    }

    this->wheels[2]->AddTransmissionTorque(transmissionTorque);
    this->wheels[3]->AddTransmissionTorque(transmissionTorque);
}

//------------------------------------------------------------------------------
/**
*/
void
Vehicle::SetBrakes(float brakes)
{
    const float brakeTorque = 4.0f;

    // apply brake torque apposing wheel velocity
    unsigned int i;
    for (i = 0; i < 4; ++i)
    {
        this->wheels[i]->AddTransmissionTorque(-this->wheels[i]->GetWheelSpeed() * brakeTorque * brakes);
    }
}

//------------------------------------------------------------------------------
/**
*/
void
Vehicle::Update(float duration)
{
    unsigned int i;
    for (i = 0; i < 4; ++i)
    {
        Vector3 worldWheelOffset = RigidBody::LocalToWorld(this->wheels[i]->GetAttachPoint());
        Vector3 worldGroundVel = RigidBody::GetVelocityAtPoint(worldWheelOffset);
        Vector3 relativeGroundSpeed = RigidBody::WorldToLocal(worldGroundVel);
        Vector3 relativeResponseForce = this->wheels[i]->CalculateForce(relativeGroundSpeed, duration);
        Vector3 worldResponseForce = RigidBody::LocalToWorld(relativeResponseForce);
        RigidBody::AddForceAtPoint(worldResponseForce, worldWheelOffset);
    }

    RigidBody::Integrate(duration);
}

} // namespace physics
} // namespace chrissly