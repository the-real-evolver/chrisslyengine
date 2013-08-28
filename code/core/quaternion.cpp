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
Quaternion::Quaternion() : w(1.0f), x(0.0f), y(0.0f), z(0.0f)
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
*/
void
Quaternion::ToRotationMatrix(Matrix3& kRot) const
{
    float fTx  = x + x;
    float fTy  = y + y;
    float fTz  = z + z;
    float fTwx = fTx * w;
    float fTwy = fTy * w;
    float fTwz = fTz * w;
    float fTxx = fTx * x;
    float fTxy = fTy * x;
    float fTxz = fTz * x;
    float fTyy = fTy * y;
    float fTyz = fTz * y;
    float fTzz = fTz * z;

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
        w * rkQ.w - x * rkQ.x - y * rkQ.y - z * rkQ.z,
        w * rkQ.x + x * rkQ.w + y * rkQ.z - z * rkQ.y,
        w * rkQ.y + y * rkQ.w + z * rkQ.x - x * rkQ.z,
        w * rkQ.z + z * rkQ.w + x * rkQ.y - y * rkQ.x
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
Quaternion::FromAngleAxis(float rfAngle, const Vector3& rkAxis)
{
    // assert:  axis[] is unit length
    //
    // The quaternion representing the rotation is
    //   q = cos(A/2) + sin(A/2) * (x*i+y*j+z*k)
    
    float fHalfAngle = 0.5f * rfAngle;
    float fSin = chrissly::core::Math::Sin(fHalfAngle);
    this->w = chrissly::core::Math::Cos(fHalfAngle);
    this->x = fSin * rkAxis.x;
    this->y = fSin * rkAxis.y;
    this->z = fSin * rkAxis.z;
}

//------------------------------------------------------------------------------
/**
*/
void
Quaternion::Normalise()
{
    float len = this->Norm();
    float factor = 1.0f / chrissly::core::Math::Sqrt(len);
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
