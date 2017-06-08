//------------------------------------------------------------------------------
//  matrix3.cpp
//  (C) 2011 Christian Bleicher
//------------------------------------------------------------------------------
#include "matrix3.h"

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
Matrix3::operator[] (size_t iRow) const
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

} // namespace core
} // namespace chrissly