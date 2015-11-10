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
float*
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

    kNeg[0][0] = -this->m[0][0]; kNeg[0][1] = -this->m[0][1]; kNeg[0][2] = -this->m[0][2];
    kNeg[1][0] = -this->m[1][0]; kNeg[1][1] = -this->m[1][1]; kNeg[1][2] = -this->m[1][2];
    kNeg[2][0] = -this->m[2][0]; kNeg[2][1] = -this->m[2][1]; kNeg[2][2] = -this->m[2][2];

    return kNeg;
}


//------------------------------------------------------------------------------
/**
*/
Vector3
Matrix3::operator * (const Vector3& rkPoint) const
{
    Vector3 kProd;

    kProd[0] = this->m[0][0] * rkPoint[0] + this->m[0][1] * rkPoint[1] + this->m[0][2] * rkPoint[2];
    kProd[1] = this->m[1][0] * rkPoint[0] + this->m[1][1] * rkPoint[1] + this->m[1][2] * rkPoint[2];
    kProd[2] = this->m[2][0] * rkPoint[0] + this->m[2][1] * rkPoint[1] + this->m[2][2] * rkPoint[2];

    return kProd;
}

//------------------------------------------------------------------------------
/**
*/
Matrix3
Matrix3::Transpose() const
{
    Matrix3 kTranspose;

    kTranspose[0][0] = this->m[0][0];
    kTranspose[0][1] = this->m[1][0];
    kTranspose[0][2] = this->m[2][0];
    kTranspose[1][0] = this->m[0][1];
    kTranspose[1][1] = this->m[1][1];
    kTranspose[1][2] = this->m[2][1];
    kTranspose[2][0] = this->m[0][2];
    kTranspose[2][1] = this->m[1][2];
    kTranspose[2][2] = this->m[2][2];

    return kTranspose;
}

} // namespace core
} // namespace chrissly