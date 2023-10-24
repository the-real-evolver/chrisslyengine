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
    float trace = rot[0U][0U] + rot[1U][1U] + rot[2U][2U];
    float root;

    if (trace > 0.0f)
    {
        root = Math::Sqrt(trace + 1.0f);
        this->w = 0.5f * root;
        root = 0.5f / root;
        this->x = (rot[2U][1U] - rot[1U][2U]) * root;
        this->y = (rot[0U][2U] - rot[2U][0U]) * root;
        this->z = (rot[1U][0U] - rot[0U][1U]) * root;
    }
    else
    {
        static unsigned int NextIndex[3U] = {1U, 2U, 0U};
        unsigned int i = 0U;
        if (rot[1U][1U] > rot[0U][0U])
        {
            i = 1U;
        }
        if (rot[2U][2U] > rot[i][i])
        {
            i = 2U;
        }
        unsigned int j = NextIndex[i];
        unsigned int k = NextIndex[j];
        root = Math::Sqrt(rot[i][i] - rot[j][j] - rot[k][k] + 1.0f);
        float* quatAxis[3U] = {&this->x, &this->y, &this->z};
        *quatAxis[i] = 0.5f * root;
        root = 0.5f / root;
        this->w = (rot[k][j] - rot[j][k]) * root;
        *quatAxis[j] = (rot[j][i] + rot[i][j]) * root;
        *quatAxis[k] = (rot[k][i]+ rot[i][k]) * root;
    }
    this->Normalise();
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

    kRot[0U][0U] = 1.0f - (fTyy + fTzz);
    kRot[0U][1U] = fTxy - fTwz;
    kRot[0U][2U] = fTxz + fTwy;
    kRot[1U][0U] = fTxy + fTwz;
    kRot[1U][1U] = 1.0f - (fTxx + fTzz);
    kRot[1U][2U] = fTyz - fTwx;
    kRot[2U][0U] = fTxz - fTwy;
    kRot[2U][1U] = fTyz + fTwx;
    kRot[2U][2U] = 1.0f - (fTxx + fTyy);
}

//------------------------------------------------------------------------------
/**
*/
void
Quaternion::FromAxes(const Vector3& xAxis, const Vector3& yAxis, const Vector3& zAxis)
{
    Matrix3 rot;

    rot[0U][0U] = xAxis.x;
    rot[1U][0U] = xAxis.y;
    rot[2U][0U] = xAxis.z;

    rot[0U][1U] = yAxis.x;
    rot[1U][1U] = yAxis.y;
    rot[2U][1U] = yAxis.z;

    rot[0U][2U] = zAxis.x;
    rot[1U][2U] = zAxis.y;
    rot[2U][2U] = zAxis.z;

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
Quaternion::operator + (const Quaternion& rkQ) const
{
    return Quaternion(this->w + rkQ.w, this->x + rkQ.x, this->y + rkQ.y, this->z + rkQ.z);
}

//------------------------------------------------------------------------------
/**
*/
Quaternion
Quaternion::operator - (const Quaternion& rkQ) const
{
    return Quaternion(this->w - rkQ.w, this->x - rkQ.x, this->y - rkQ.y, this->z - rkQ.z);
}

//------------------------------------------------------------------------------
/**
*/
Quaternion
Quaternion::operator * (const Quaternion& rkQ) const
{
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
Quaternion
operator * (float fScalar, const Quaternion& rkQ)
{
    return Quaternion(fScalar * rkQ.w, fScalar * rkQ.x, fScalar * rkQ.y, fScalar * rkQ.z);
}

//------------------------------------------------------------------------------
/**
*/
Quaternion
Quaternion::operator - () const
{
    return Quaternion(-this->w, -this->x, -this->y, -this->z);
}

//------------------------------------------------------------------------------
/**
*/
float
Quaternion::Dot(const Quaternion& rkQ) const
{
    return this->w * rkQ.w + this->x * rkQ.x + this->y * rkQ.y + this->z * rkQ.z;
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
Quaternion::Nlerp(const Quaternion& rkP, const Quaternion& rkQ, float t, bool shortestPath)
{
    Quaternion result;
    float fCos = rkP.Dot(rkQ);
    if (fCos < 0.0f && shortestPath)
    {
        result = rkP + ((-rkQ) - rkP) * t;
    }
    else
    {
        result = rkP + (rkQ - rkP) * t;
    }
    result.Normalise();
    return result;
}

//------------------------------------------------------------------------------
/**
*/
Quaternion
Quaternion::Slerp(const Quaternion& rkP, const Quaternion& rkQ, float t, bool shortestPath)
{
    float fCos = rkP.Dot(rkQ);
    Quaternion rkT;

    // do we need to invert rotation?
    if (fCos < 0.0f && shortestPath)
    {
        fCos = -fCos;
        rkT = -rkQ;
    }
    else
    {
        rkT = rkQ;
    }

    if (Math::Fabs(fCos) < 1.0f - 1e-03f)
    {
        // standard case (slerp)
        float fSin = Math::Sqrt(1.0f - Math::Sqrt(fCos));
        float fAngle = Math::ATan2(fSin, fCos);
        float fInvSin = 1.0f / fSin;
        float fCoeff0 = Math::Sin((1.0f - t) * fAngle) * fInvSin;
        float fCoeff1 = Math::Sin(t * fAngle) * fInvSin;
        return fCoeff0 * rkP + fCoeff1 * rkT;
    }
    else
    {
        // There are two situations:
        // 1. "rkP" and "rkQ" are very close (fCos ~= +1), so we can do a linear
        //    interpolation safely
        // 2. "rkP" and "rkQ" are almost inverse of each other (fCos ~= -1), there
        //    are an infinite number of possibilities interpolation. but we haven't
        //    have method to fix this case, so just use linear interpolation here
        Quaternion qt = (1.0f - t) * rkP + t * rkT;
        // taking the complement requires renormalisation
        qt.Normalise();
        return qt;
    }
}

} // namespace core
} // namespace chrissly
