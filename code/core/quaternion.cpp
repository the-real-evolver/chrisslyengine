//------------------------------------------------------------------------------
//  quaternion.cpp
//  (C) 2011 Christian Bleicher
//------------------------------------------------------------------------------
#include "quaternion.h"
#include "chrisslymath.h"

namespace chrissly
{
namespace core
{

//------------------------------------------------------------------------------
/**
*/
Quaternion::Quaternion() :
    w(1.0f),
    x(0.0f),
    y(0.0f),
    z(0.0f)
{

}

//------------------------------------------------------------------------------
/**
*/
Quaternion::Quaternion(float w, float x, float y, float z)
{
    this->w = w;
    this->x = x;
    this->y = y;
    this->z = z;
}

//------------------------------------------------------------------------------
/**
*/
Quaternion::~Quaternion()
{

}

//------------------------------------------------------------------------------
/**
    Algorithm from Ken Shoemake's "Quaternions" tutorial:

    "Construct a unit quaternion from rotation matrix. Assumes matrix is used to
    multiply column vector on the left: vnew = mat vold. Works correctly for
    right-handed coordinate system and right-handed rotations. Translation and
    perspective components ignored.

    This algorithm avoids near-zero divides by looking for a large component -
    first w, then x, y, or z. When the trace is greater than zero, |w| is
    greater than 1/2, which is as small as a largest component can be. Otherwise
    , the largest diagonal entry corresponds to the largest of |x|, |y|, or |z|,
    one of which must be larger than |w|, and at least 1/2."
*/
void
Quaternion::FromRotationMatrix(const Matrix3& rot)
{
    float trace = rot[0][0] + rot[1][1] + rot[2][2];
    float root;

    if (trace > 0.0f)
    {
        root = Math::Sqrt(trace + 1.0f);
        this->w = 0.5f * root;
        root = 0.5f / root;
        this->x = (rot[2][1] - rot[1][2]) * root;
        this->y = (rot[0][2] - rot[2][0]) * root;
        this->z = (rot[1][0] - rot[0][1]) * root;
    }
    else
    {
        static unsigned int NextIndex[3] = {1, 2, 0};
        unsigned int i = 0U;
        if (rot[1][1] > rot[0][0])
        {
            i = 1U;
        }
        if (rot[2][2] > rot[i][i])
        {
            i = 2U;
        }
        unsigned int j = NextIndex[i];
        unsigned int k = NextIndex[j];
        root = Math::Sqrt(rot[i][i] - rot[j][j] - rot[k][k] + 1.0f);
        float* quatAxis[3] = {&this->x, &this->y, &this->z};
        *quatAxis[i] = 0.5f * root;
        root = 0.5f / root;
        this->w = (rot[k][j] - rot[j][k]) * root;
        *quatAxis[j] = (rot[j][i] + rot[i][j]) * root;
        *quatAxis[k] = (rot[k][i]+ rot[i][k]) * root;
    }
}

//------------------------------------------------------------------------------
/**
*/
void
Quaternion::ToRotationMatrix(Matrix3& kRot) const
{
    float fTx  = this->x + this->x;
    float fTy  = this->y + this->y;
    float fTz  = this->z + this->z;
    float fTwx = fTx * this->w;
    float fTwy = fTy * this->w;
    float fTwz = fTz * this->w;
    float fTxx = fTx * this->x;
    float fTxy = fTy * this->x;
    float fTxz = fTz * this->x;
    float fTyy = fTy * this->y;
    float fTyz = fTz * this->y;
    float fTzz = fTz * this->z;

    kRot[0][0] = 1.0f - (fTyy + fTzz);
    kRot[0][1] = fTxy - fTwz;
    kRot[0][2] = fTxz + fTwy;
    kRot[1][0] = fTxy + fTwz;
    kRot[1][1] = 1.0f - (fTxx + fTzz);
    kRot[1][2] = fTyz - fTwx;
    kRot[2][0] = fTxz - fTwy;
    kRot[2][1] = fTyz + fTwx;
    kRot[2][2] = 1.0f - (fTxx + fTyy);
}

//------------------------------------------------------------------------------
/**
*/
void
Quaternion::FromAxes(const Vector3& xAxis, const Vector3& yAxis, const Vector3& zAxis)
{
    Matrix3 rot;

    rot[0][0] = xAxis.x;
    rot[1][0] = xAxis.y;
    rot[2][0] = xAxis.z;

    rot[0][1] = yAxis.x;
    rot[1][1] = yAxis.y;
    rot[2][1] = yAxis.z;

    rot[0][2] = zAxis.x;
    rot[1][2] = zAxis.y;
    rot[2][2] = zAxis.z;

    this->FromRotationMatrix(rot);
}

//------------------------------------------------------------------------------
/**
*/
void
Quaternion::FromAngleAxis(float rfAngle, const Vector3& rkAxis)
{
    // assert:  axis[] is unit length
    //
    // The quaternion representing the rotation is
    //   q = cos(A/2) + sin(A/2) * (x*i+y*j+z*k)

    float fHalfAngle = 0.5f * rfAngle;
    float fSin = Math::Sin(fHalfAngle);
    this->w = Math::Cos(fHalfAngle);
    this->x = fSin * rkAxis.x;
    this->y = fSin * rkAxis.y;
    this->z = fSin * rkAxis.z;
}

//------------------------------------------------------------------------------
/**
*/
Quaternion&
Quaternion::operator = (const Quaternion& rkQ)
{
    this->w = rkQ.w;
    this->x = rkQ.x;
    this->y = rkQ.y;
    this->z = rkQ.z;

    return *this;
}

//------------------------------------------------------------------------------
/**
*/
Quaternion
Quaternion::operator * (const Quaternion& rkQ) const
{
    /* 
        float ww = (q1.zp + q1.xp) * (q2.xp + q2.yp);
        float yy = (q1.wp - q1.yp) * (q2.wp + q2.zp);
        float zz = (q1.wp + q1.yp) * (q2.wp - q2.zp);
        float xx = ww + yy + zz;
        float qq = 0.5 * (xx + (q1.zp - q1.xp) * (q2.xp - q2.yp));

        float w = qq - ww + (q1.zp - q1.yp) * (q2.yp - q2.zp);
        float x = qq - xx + (q1.xp + q1.wp) * (q2.xp + q2.wp);
        float y = qq - yy + (q1.wp - q1.xp) * (q2.yp + q2.zp);
        float z = qq - zz + (q1.zp + q1.yp) * (q2.wp - q2.xp);
    */

    return Quaternion
    (
        this->w * rkQ.w - this->x * rkQ.x - this->y * rkQ.y - this->z * rkQ.z,
        this->w * rkQ.x + this->x * rkQ.w + this->y * rkQ.z - this->z * rkQ.y,
        this->w * rkQ.y + this->y * rkQ.w + this->z * rkQ.x - this->x * rkQ.z,
        this->w * rkQ.z + this->z * rkQ.w + this->x * rkQ.y - this->y * rkQ.x
    );
}

//------------------------------------------------------------------------------
/**
*/
Vector3
Quaternion::operator * (const Vector3& rkVector) const
{
    // nVidia SDK implementation
    Vector3 uv, uuv;
    Vector3 qvec(this->x, this->y, this->z);

    uv = qvec.CrossProduct(rkVector);
    uuv = qvec.CrossProduct(uv);
    uv *= (2.0f * this->w);
    uuv *= 2.0f;

    return rkVector + uv + uuv;
}

//------------------------------------------------------------------------------
/**
*/
Quaternion
Quaternion::operator * (float fScalar) const
{
    return Quaternion(fScalar * this->w, fScalar * this->x, fScalar * this->y, fScalar * this->z);
}

//------------------------------------------------------------------------------
/**
*/
void
Quaternion::Normalise()
{
    float len = this->Norm();
    float factor = 1.0f / Math::Sqrt(len);
    *this = *this * factor;
}

//------------------------------------------------------------------------------
/**
*/
float
Quaternion::Norm() const
{
    return this->w * this->w + this->x * this->x + this->y * this->y + this->z * this->z;
}

//------------------------------------------------------------------------------
/**
*/
Quaternion
Quaternion::Inverse() const
{
    float fNorm = this->Norm();
    if (fNorm > 0.0f)
    {
        float fInvNorm = 1.0f / fNorm;
        return Quaternion(this->w * fInvNorm, -this->x * fInvNorm, -this->y * fInvNorm, -this->z * fInvNorm);
    }
    else
    {
        // return an invalid result to flag the error
        return Quaternion(0.0f, 0.0f, 0.0f, 0.0f);
    }
}

} // namespace core
} // namespace chrissly
