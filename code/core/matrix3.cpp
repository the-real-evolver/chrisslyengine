//------------------------------------------------------------------------------
//  matrix3.cpp
//  (C) 2011 Christian Bleicher
//------------------------------------------------------------------------------
#include "matrix3.h"
#include "chrisslymath.h"

namespace chrissly
{
namespace core
{

//------------------------------------------------------------------------------
/**
*/
Matrix3::Matrix3()
{

}

//------------------------------------------------------------------------------
/**
*/
Matrix3::~Matrix3()
{

}

//------------------------------------------------------------------------------
/**
*/
float* const
Matrix3::operator[] (unsigned int iRow) const
{
    return (float*)m[iRow];
}

//------------------------------------------------------------------------------
/**
*/
Matrix3
Matrix3::operator - () const
{
    Matrix3 kNeg;

    kNeg[0U][0U] = -this->m[0U][0U]; kNeg[0U][1U] = -this->m[0U][1U]; kNeg[0U][2U] = -this->m[0U][2U];
    kNeg[1U][0U] = -this->m[1U][0U]; kNeg[1U][1U] = -this->m[1U][1U]; kNeg[1U][2U] = -this->m[1U][2U];
    kNeg[2U][0U] = -this->m[2U][0U]; kNeg[2U][1U] = -this->m[2U][1U]; kNeg[2U][2U] = -this->m[2U][2U];

    return kNeg;
}

//------------------------------------------------------------------------------
/**
*/
Matrix3
Matrix3::operator*(const Matrix3& m2) const
{
    Matrix3 r;
    r[0U][0U] = m[0U][0U] * m2[0U][0U] + m[0U][1U] * m2[1U][0U] + m[0U][2U] * m2[2U][0U];
    r[0U][1U] = m[0U][0U] * m2[0U][1U] + m[0U][1U] * m2[1U][1U] + m[0U][2U] * m2[2U][1U];
    r[0U][2U] = m[0U][0U] * m2[0U][2U] + m[0U][1U] * m2[1U][2U] + m[0U][2U] * m2[2U][2U];

    r[1U][0U] = m[1U][0U] * m2[0U][0U] + m[1U][1U] * m2[1U][0U] + m[1U][2U] * m2[2U][0U];
    r[1U][1U] = m[1U][0U] * m2[0U][1U] + m[1U][1U] * m2[1U][1U] + m[1U][2U] * m2[2U][1U];
    r[1U][2U] = m[1U][0U] * m2[0U][2U] + m[1U][1U] * m2[1U][2U] + m[1U][2U] * m2[2U][2U];

    r[2U][0U] = m[2U][0U] * m2[0U][0U] + m[2U][1U] * m2[1U][0U] + m[2U][2U] * m2[2U][0U];
    r[2U][1U] = m[2U][0U] * m2[0U][1U] + m[2U][1U] * m2[1U][1U] + m[2U][2U] * m2[2U][1U];
    r[2U][2U] = m[2U][0U] * m2[0U][2U] + m[2U][1U] * m2[1U][2U] + m[2U][2U] * m2[2U][2U];

    return r;
}

//------------------------------------------------------------------------------
/**
*/
Vector3
Matrix3::operator * (const Vector3& rkPoint) const
{
    Vector3 kProd;

    kProd[0U] = this->m[0U][0U] * rkPoint[0U] + this->m[0U][1U] * rkPoint[1U] + this->m[0U][2U] * rkPoint[2U];
    kProd[1U] = this->m[1U][0U] * rkPoint[0U] + this->m[1U][1U] * rkPoint[1U] + this->m[1U][2U] * rkPoint[2U];
    kProd[2U] = this->m[2U][0U] * rkPoint[0U] + this->m[2U][1U] * rkPoint[1U] + this->m[2U][2U] * rkPoint[2U];

    return kProd;
}

//------------------------------------------------------------------------------
/**
*/
Matrix3
Matrix3::Transpose() const
{
    Matrix3 kTranspose;

    kTranspose[0U][0U] = this->m[0U][0U];
    kTranspose[0U][1U] = this->m[1U][0U];
    kTranspose[0U][2U] = this->m[2U][0U];
    kTranspose[1U][0U] = this->m[0U][1U];
    kTranspose[1U][1U] = this->m[1U][1U];
    kTranspose[1U][2U] = this->m[2U][1U];
    kTranspose[2U][0U] = this->m[0U][2U];
    kTranspose[2U][1U] = this->m[1U][2U];
    kTranspose[2U][2U] = this->m[2U][2U];

    return kTranspose;
}

//------------------------------------------------------------------------------
/**
*/
bool
Matrix3::ToEulerAngles(float& yaw, float& pitch, float& roll) const
{
    pitch = Math::ASin(-this->m[1U][2U]);
    if (pitch < (float)M_PI_2)
    {
        if (pitch > (float)-M_PI_2)
        {
            yaw = Math::ATan2(this->m[0U][2U], this->m[2U][2U]);
            roll = Math::ATan2(this->m[1U][0U], this->m[1U][1U]);
            return true;
        }
        else
        {
            // no unique solution
            float fRmY = Math::ATan2(-this->m[0U][1U], this->m[0U][0U]);
            roll = 0.0f; // any angle works
            yaw = roll - fRmY;
            return false;
        }
    }
    else
    {
        // no unique solution
        float fRpY = Math::ATan2(-this->m[0U][1U], this->m[0U][0U]);
        roll = 0.0f; // any angle works
        yaw = fRpY - roll;
        return false;
    }
}

} // namespace core
} // namespace chrissly