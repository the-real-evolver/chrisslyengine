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
    size_t iRow;
    for (iRow = 0; iRow < 3; ++iRow)
    {
        for (size_t iCol = 0; iCol < 3; ++iCol)
        {
            kNeg[iRow][iCol] = -this->m[iRow][iCol];
        }
    }
    
    return kNeg;
}


//------------------------------------------------------------------------------
/**
*/
Vector3
Matrix3::operator * (const Vector3& rkPoint) const
{
    Vector3 kProd;
    size_t iRow;
    for (iRow = 0; iRow < 3; ++iRow)
    {
        kProd[iRow] = this->m[iRow][0] * rkPoint[0] + this->m[iRow][1] * rkPoint[1] + this->m[iRow][2] * rkPoint[2];
    }

    return kProd;
}

//------------------------------------------------------------------------------
/**
*/
Matrix3
Matrix3::Transpose() const
{
    Matrix3 kTranspose;
    size_t iRow;
    for (iRow = 0; iRow < 3; ++iRow)
    {
        for (size_t iCol = 0; iCol < 3; ++iCol)
        {
            kTranspose[iRow][iCol] = this->m[iCol][iRow];
        }
    }

    return kTranspose;
}

} // namespace core
} // namespace chrissly