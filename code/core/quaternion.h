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
    /// update from rotationmatrix
    void FromRotationMatrix(const Matrix3& rot);
    /// construct rotationmatrix from quaternion
    void ToRotationMatrix(Matrix3& kRot) const;
    /// updates the quaternion from 3 orthonormal axes
    void FromAxes(const Vector3& xAxis, const Vector3& yAxis, const Vector3& zAxis);
    /// update from angle and axis
    void FromAngleAxis(float rfAngle, const Vector3& rkAxis);
    /// assign operator
    Quaternion& operator = (const Quaternion& rkQ);
    /// quaternion addition
    Quaternion operator + (const Quaternion& rkQ) const;
    /// quaternion subtraction
    Quaternion operator - (const Quaternion& rkQ) const;
    /// quaternion multiplication
    Quaternion operator * (const Quaternion& rkQ) const;
    /// rotation of a vector by a quaternion
    Vector3 operator * (const Vector3& rkVector) const;
    /// multiply by a scalar
    Quaternion operator * (float fScalar) const;
    friend Quaternion operator * (float fScalar, const Quaternion& rkQ);
    /// unary minus operator
    Quaternion operator - () const;
    /// returns the dot product between this and the given quaternion
    float Dot(const Quaternion& rkQ) const;
    /// normalises this quaternion
    void Normalise();
    /// squared-length
    float Norm() const;
    /// returns a Quaternion with the inverse orientation of this Quaternion
    Quaternion Inverse() const;
    /// returns normalised linear interpolation between the two given quaternions
    static Quaternion Nlerp(const Quaternion& rkP, const Quaternion& rkQ, float t, bool shortestPath = false);
    /// spherical linear interpolation between two quaternions
    static Quaternion Slerp(const Quaternion& rkP, const Quaternion& rkQ, float t, bool shortestPath = false);

    float w;
    float x;
    float y;
    float z;
};

} // namespace core
} // namespace chrissly
//------------------------------------------------------------------------------
#endif
