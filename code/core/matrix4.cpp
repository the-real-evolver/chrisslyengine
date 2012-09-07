//------------------------------------------------------------------------------
//  matrix4.cpp
//  (C) 2012 Christian Bleicher
//------------------------------------------------------------------------------
#include "matrix4.h"

namespace chrissly
{
namespace core
{

const Matrix4 Matrix4::IDENTITY(1.0f, 0.0f, 0.0f, 0.0f,
                                0.0f, 1.0f, 0.0f, 0.0f,
                                0.0f, 0.0f, 1.0f, 0.0f,
                                0.0f, 0.0f, 0.0f, 1.0f);

const Matrix4 Matrix4::ZERO(0.0f, 0.0f, 0.0f, 0.0f,
                            0.0f, 0.0f, 0.0f, 0.0f,
                            0.0f, 0.0f, 0.0f, 0.0f,
                            0.0f, 0.0f, 0.0f, 0.0f);

//------------------------------------------------------------------------------
/**
*/
Matrix4::Matrix4()
{

}

//------------------------------------------------------------------------------
/**
*/
Matrix4::Matrix4(float m00, float m01, float m02, float m03,
                    float m10, float m11, float m12, float m13,
                    float m20, float m21, float m22, float m23,
                    float m30, float m31, float m32, float m33)
{
    this->m[0][0] = m00;
    this->m[0][1] = m01;
    this->m[0][2] = m02;
    this->m[0][3] = m03;
    this->m[1][0] = m10;
    this->m[1][1] = m11;
    this->m[1][2] = m12;
    this->m[1][3] = m13;
    this->m[2][0] = m20;
    this->m[2][1] = m21;
    this->m[2][2] = m22;
    this->m[2][3] = m23;
    this->m[3][0] = m30;
    this->m[3][1] = m31;
    this->m[3][2] = m32;
    this->m[3][3] = m33;
}

//------------------------------------------------------------------------------
/**
*/
Matrix4::~Matrix4()
{
  
}

//------------------------------------------------------------------------------
/**
*/
Matrix4
Matrix4::Concatenate(const Matrix4& m2) const
{
    Matrix4 r;
    r.m[0][0] = this->m[0][0] * m2.m[0][0] + this->m[0][1] * m2.m[1][0] + this->m[0][2] * m2.m[2][0] + this->m[0][3] * m2.m[3][0];
    r.m[0][1] = this->m[0][0] * m2.m[0][1] + this->m[0][1] * m2.m[1][1] + this->m[0][2] * m2.m[2][1] + this->m[0][3] * m2.m[3][1];
    r.m[0][2] = this->m[0][0] * m2.m[0][2] + this->m[0][1] * m2.m[1][2] + this->m[0][2] * m2.m[2][2] + this->m[0][3] * m2.m[3][2];
    r.m[0][3] = this->m[0][0] * m2.m[0][3] + this->m[0][1] * m2.m[1][3] + this->m[0][2] * m2.m[2][3] + this->m[0][3] * m2.m[3][3];

    r.m[1][0] = this->m[1][0] * m2.m[0][0] + this->m[1][1] * m2.m[1][0] + this->m[1][2] * m2.m[2][0] + this->m[1][3] * m2.m[3][0];
    r.m[1][1] = this->m[1][0] * m2.m[0][1] + this->m[1][1] * m2.m[1][1] + this->m[1][2] * m2.m[2][1] + this->m[1][3] * m2.m[3][1];
    r.m[1][2] = this->m[1][0] * m2.m[0][2] + this->m[1][1] * m2.m[1][2] + this->m[1][2] * m2.m[2][2] + this->m[1][3] * m2.m[3][2];
    r.m[1][3] = this->m[1][0] * m2.m[0][3] + this->m[1][1] * m2.m[1][3] + this->m[1][2] * m2.m[2][3] + this->m[1][3] * m2.m[3][3];

    r.m[2][0] = this->m[2][0] * m2.m[0][0] + this->m[2][1] * m2.m[1][0] + this->m[2][2] * m2.m[2][0] + this->m[2][3] * m2.m[3][0];
    r.m[2][1] = this->m[2][0] * m2.m[0][1] + this->m[2][1] * m2.m[1][1] + this->m[2][2] * m2.m[2][1] + this->m[2][3] * m2.m[3][1];
    r.m[2][2] = this->m[2][0] * m2.m[0][2] + this->m[2][1] * m2.m[1][2] + this->m[2][2] * m2.m[2][2] + this->m[2][3] * m2.m[3][2];
    r.m[2][3] = this->m[2][0] * m2.m[0][3] + this->m[2][1] * m2.m[1][3] + this->m[2][2] * m2.m[2][3] + this->m[2][3] * m2.m[3][3];

    r.m[3][0] = this->m[3][0] * m2.m[0][0] + this->m[3][1] * m2.m[1][0] + this->m[3][2] * m2.m[2][0] + this->m[3][3] * m2.m[3][0];
    r.m[3][1] = this->m[3][0] * m2.m[0][1] + this->m[3][1] * m2.m[1][1] + this->m[3][2] * m2.m[2][1] + this->m[3][3] * m2.m[3][1];
    r.m[3][2] = this->m[3][0] * m2.m[0][2] + this->m[3][1] * m2.m[1][2] + this->m[3][2] * m2.m[2][2] + this->m[3][3] * m2.m[3][2];
    r.m[3][3] = this->m[3][0] * m2.m[0][3] + this->m[3][1] * m2.m[1][3] + this->m[3][2] * m2.m[2][3] + this->m[3][3] * m2.m[3][3];

    return r;
}

//------------------------------------------------------------------------------
/**
*/
Matrix4
Matrix4::operator * (const Matrix4 &m2) const
{
    return this->Concatenate(m2);
}

//------------------------------------------------------------------------------
/**
*/
float*
Matrix4::operator[] (size_t iRow) const
{
    return (float*)this->m[iRow];
}

} // namespace core
} // namespace chrissly