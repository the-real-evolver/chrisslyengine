//------------------------------------------------------------------------------
//  wheel.cpp
//  (C) 2010 Christian Bleicher
//------------------------------------------------------------------------------
#include "wheel.h"
#include "chrisslymath.h"

namespace chrissly
{
namespace physics
{

using namespace chrissly::core;

//------------------------------------------------------------------------------
/**
*/
Wheel::Wheel()
{

}

//------------------------------------------------------------------------------
/**
*/
Wheel::Wheel(const Vector3& position, float radius)
{
    this->position = position;
    this->wheelRadius = radius;
    this->wheelSpeed = 0.0f;
    this->wheelTorque = 0.0f;
    this->wheelInertia = radius * radius;
    this->SetSteeringAngle(0.0f);
}

//------------------------------------------------------------------------------
/**
*/
Wheel::~Wheel()
{

}

//------------------------------------------------------------------------------
/**
*/
void
Wheel::SetSteeringAngle(float newAngle)
{
    Vector3 forwardVector;
    Vector3 sideVector;

    forwardVector.x = 0.0f;
    forwardVector.z = 1.0f;
    sideVector.x = -1.0f;
    sideVector.z = 0.0f;

    float sine = Math::Sin(newAngle);
    float cosine = Math::Cos(newAngle);

    this->forwardAxis.x = forwardVector.x * cosine - forwardVector.z * sine;
    this->forwardAxis.z = forwardVector.x * sine   + forwardVector.z * cosine;
    this->sideAxis.x = sideVector.x * cosine - sideVector.z * sine;
    this->sideAxis.z = sideVector.x * sine   + sideVector.z * cosine;
}

//------------------------------------------------------------------------------
/**
*/
void
Wheel::AddTransmissionTorque(float newValue)
{
    this->wheelTorque += newValue;
}

//------------------------------------------------------------------------------
/**
*/
float
Wheel::GetWheelSpeed()
{
    return this->wheelSpeed;
}

//------------------------------------------------------------------------------
/**
*/
const Vector3&
Wheel::GetAttachPoint()
{
    return this->position;
}

//------------------------------------------------------------------------------
/**
*/
Vector3
Wheel::CalculateForce(const Vector3& relativeGroundSpeed, float timeStep)
{
    // calculate speed of tire patch at ground
    Vector3 patchSpeed;
    patchSpeed.x = -this->forwardAxis.x * this->wheelSpeed * this->wheelRadius;
    patchSpeed.z = -this->forwardAxis.z * this->wheelSpeed * this->wheelRadius;

    // get velocity difference between ground and patch
    Vector3 velDifference;
    velDifference.x = relativeGroundSpeed.x + patchSpeed.x;
    velDifference.z = relativeGroundSpeed.z + patchSpeed.z;

    // project ground speed onto side axis
    Vector3 sideVel;
    float thisDotV = velDifference.x * this->sideAxis.x + velDifference.z * this->sideAxis.z;
    sideVel.x = this->sideAxis.x * thisDotV;
    sideVel.z = this->sideAxis.z * thisDotV;

    Vector3 forwardVel;
    thisDotV = velDifference.x * this->forwardAxis.x + velDifference.z * this->forwardAxis.z;
    forwardVel.x = this->forwardAxis.x * thisDotV;
    forwardVel.z = this->forwardAxis.z * thisDotV;

    // calculate super fake friction forces and response force
    Vector3 responseForce;
    responseForce.x = -sideVel.x * 2.0f - forwardVel.x;
    responseForce.z = -sideVel.z * 2.0f - forwardVel.z;

    // calculate torque on wheel
    this->wheelTorque += thisDotV * this->wheelRadius;

    // integrate total torque into wheel
    this->wheelSpeed += (this->wheelTorque / this->wheelInertia) * timeStep;

    // clear our transmission torque accumulator
    this->wheelTorque = 0.0f;

    // return force acting on body
    return responseForce;
}

} // namespace physics
} // namespace chrissly