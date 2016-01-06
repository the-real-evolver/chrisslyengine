#ifndef QUATERNION_H_
#define QUATERNION_H_
//------------------------------------------------------------------------------
/**
    @class chrissly::core::Quaternion

    (C) 2011 Christian Bleicher
*/
#include "vector3.h"
#include "matrix3.h"

//------------------------------------------------------------------------------
namespace chrissly
{
namespace core
{

class Quaternion
{
public:
    /// default constructor
    Quaternion();
    /// construct from values
    Quaternion(float w, float x, float y, float z);
    /// destructor
    ~Quaternion();
    /// contruct rotationmatrix from quaternion
    void ToRotationMatrix(Matrix3& kRot) const;
    /// assign operator
    Quaternion& operator = (const Quaternion& rkQ);
    /// quaternion multiplication
    Quaternion operator * (const Quaternion& rkQ) const;
    /// rotation of a vector by a quaternion
    Vector3 operator * (const Vector3& rkVector) const;
    /// multiply by a scalar
    Quaternion operator * (float fScalar) const;
    /// update from angle and axis
    void FromAngleAxis(float rfAngle, const Vector3& rkAxis);
    /// normalises this quaternion
    void Normalise();
    /// squared-length
    float Norm() const;
    /// returns a Quaternion with the inverse orientation of this Quaternion
    Quaternion Inverse() const;

    float w;
    float x;
    float y;
    float z;
};

} // namespace core
} // namespace chrissly
//------------------------------------------------------------------------------
#endif
