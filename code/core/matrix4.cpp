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
    this->m[0U][0U] = m00;
    this->m[0U][1U] = m01;
    this->m[0U][2U] = m02;
    this->m[0U][3U] = m03;
    this->m[1U][0U] = m10;
    this->m[1U][1U] = m11;
    this->m[1U][2U] = m12;
    this->m[1U][3U] = m13;
    this->m[2U][0U] = m20;
    this->m[2U][1U] = m21;
    this->m[2U][2U] = m22;
    this->m[2U][3U] = m23;
    this->m[3U][0U] = m30;
    this->m[3U][1U] = m31;
    this->m[3U][2U] = m32;
    this->m[3U][3U] = m33;
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
Matrix4::Inverse() const
{
    // calcualte inverse using cramer's rule and the adjugate

    float m00 = this->m[0U][0U], m01 = this->m[0U][1U], m02 = this->m[0U][2U], m03 = this->m[0U][3U];
    float m10 = this->m[1U][0U], m11 = this->m[1U][1U], m12 = this->m[1U][2U], m13 = this->m[1U][3U];
    float m20 = this->m[2U][0U], m21 = this->m[2U][1U], m22 = this->m[2U][2U], m23 = this->m[2U][3U];
    float m30 = this->m[3U][0U], m31 = this->m[3U][1U], m32 = this->m[3U][2U], m33 = this->m[3U][3U];

    float v0 = m20 * m31 - m21 * m30;
    float v1 = m20 * m32 - m22 * m30;
    float v2 = m20 * m33 - m23 * m30;
    float v3 = m21 * m32 - m22 * m31;
    float v4 = m21 * m33 - m23 * m31;
    float v5 = m22 * m33 - m23 * m32;

    float t00 = v5 * m11 - v4 * m12 + v3 * m13;
    float t10 = -(v5 * m10 - v2 * m12 + v1 * m13);
    float t20 = v4 * m10 - v2 * m11 + v0 * m13;
    float t30 = -(v3 * m10 - v1 * m11 + v0 * m12);

    float invDet = 1.0f / (t00 * m00 + t10 * m01 + t20 * m02 + t30 * m03);

    float d00 = t00 * invDet;
    float d10 = t10 * invDet;
    float d20 = t20 * invDet;
    float d30 = t30 * invDet;

    float d01 = - (v5 * m01 - v4 * m02 + v3 * m03) * invDet;
    float d11 = (v5 * m00 - v2 * m02 + v1 * m03) * invDet;
    float d21 = - (v4 * m00 - v2 * m01 + v0 * m03) * invDet;
    float d31 = (v3 * m00 - v1 * m01 + v0 * m02) * invDet;

    v0 = m10 * m31 - m11 * m30;
    v1 = m10 * m32 - m12 * m30;
    v2 = m10 * m33 - m13 * m30;
    v3 = m11 * m32 - m12 * m31;
    v4 = m11 * m33 - m13 * m31;
    v5 = m12 * m33 - m13 * m32;

    float d02 = (v5 * m01 - v4 * m02 + v3 * m03) * invDet;
    float d12 = -(v5 * m00 - v2 * m02 + v1 * m03) * invDet;
    float d22 = (v4 * m00 - v2 * m01 + v0 * m03) * invDet;
    float d32 = -(v3 * m00 - v1 * m01 + v0 * m02) * invDet;

    v0 = m21 * m10 - m20 * m11;
    v1 = m22 * m10 - m20 * m12;
    v2 = m23 * m10 - m20 * m13;
    v3 = m22 * m11 - m21 * m12;
    v4 = m23 * m11 - m21 * m13;
    v5 = m23 * m12 - m22 * m13;

    float d03 = -(v5 * m01 - v4 * m02 + v3 * m03) * invDet;
    float d13 = (v5 * m00 - v2 * m02 + v1 * m03) * invDet;
    float d23 = -(v4 * m00 - v2 * m01 + v0 * m03) * invDet;
    float d33 = (v3 * m00 - v1 * m01 + v0 * m02) * invDet;

    return Matrix4(
        d00, d01, d02, d03,
        d10, d11, d12, d13,
        d20, d21, d22, d23,
        d30, d31, d32, d33);
}

//------------------------------------------------------------------------------
/**
*/
Matrix4
Matrix4::Concatenate(const Matrix4& m2) const
{
    Matrix4 r;
    r.m[0U][0U] = this->m[0U][0U] * m2.m[0U][0U] + this->m[0U][1U] * m2.m[1U][0U] + this->m[0U][2U] * m2.m[2U][0U] + this->m[0U][3U] * m2.m[3U][0U];
    r.m[0U][1U] = this->m[0U][0U] * m2.m[0U][1U] + this->m[0U][1U] * m2.m[1U][1U] + this->m[0U][2U] * m2.m[2U][1U] + this->m[0U][3U] * m2.m[3U][1U];
    r.m[0U][2U] = this->m[0U][0U] * m2.m[0U][2U] + this->m[0U][1U] * m2.m[1U][2U] + this->m[0U][2U] * m2.m[2U][2U] + this->m[0U][3U] * m2.m[3U][2U];
    r.m[0U][3U] = this->m[0U][0U] * m2.m[0U][3U] + this->m[0U][1U] * m2.m[1U][3U] + this->m[0U][2U] * m2.m[2U][3U] + this->m[0U][3U] * m2.m[3U][3U];

    r.m[1U][0U] = this->m[1U][0U] * m2.m[0U][0U] + this->m[1U][1U] * m2.m[1U][0U] + this->m[1U][2U] * m2.m[2U][0U] + this->m[1U][3U] * m2.m[3U][0U];
    r.m[1U][1U] = this->m[1U][0U] * m2.m[0U][1U] + this->m[1U][1U] * m2.m[1U][1U] + this->m[1U][2U] * m2.m[2U][1U] + this->m[1U][3U] * m2.m[3U][1U];
    r.m[1U][2U] = this->m[1U][0U] * m2.m[0U][2U] + this->m[1U][1U] * m2.m[1U][2U] + this->m[1U][2U] * m2.m[2U][2U] + this->m[1U][3U] * m2.m[3U][2U];
    r.m[1U][3U] = this->m[1U][0U] * m2.m[0U][3U] + this->m[1U][1U] * m2.m[1U][3U] + this->m[1U][2U] * m2.m[2U][3U] + this->m[1U][3U] * m2.m[3U][3U];

    r.m[2U][0U] = this->m[2U][0U] * m2.m[0U][0U] + this->m[2U][1U] * m2.m[1U][0U] + this->m[2U][2U] * m2.m[2U][0U] + this->m[2U][3U] * m2.m[3U][0U];
    r.m[2U][1U] = this->m[2U][0U] * m2.m[0U][1U] + this->m[2U][1U] * m2.m[1U][1U] + this->m[2U][2U] * m2.m[2U][1U] + this->m[2U][3U] * m2.m[3U][1U];
    r.m[2U][2U] = this->m[2U][0U] * m2.m[0U][2U] + this->m[2U][1U] * m2.m[1U][2U] + this->m[2U][2U] * m2.m[2U][2U] + this->m[2U][3U] * m2.m[3U][2U];
    r.m[2U][3U] = this->m[2U][0U] * m2.m[0U][3U] + this->m[2U][1U] * m2.m[1U][3U] + this->m[2U][2U] * m2.m[2U][3U] + this->m[2U][3U] * m2.m[3U][3U];

    r.m[3U][0U] = this->m[3U][0U] * m2.m[0U][0U] + this->m[3U][1U] * m2.m[1U][0U] + this->m[3U][2U] * m2.m[2U][0U] + this->m[3U][3U] * m2.m[3U][0U];
    r.m[3U][1U] = this->m[3U][0U] * m2.m[0U][1U] + this->m[3U][1U] * m2.m[1U][1U] + this->m[3U][2U] * m2.m[2U][1U] + this->m[3U][3U] * m2.m[3U][1U];
    r.m[3U][2U] = this->m[3U][0U] * m2.m[0U][2U] + this->m[3U][1U] * m2.m[1U][2U] + this->m[3U][2U] * m2.m[2U][2U] + this->m[3U][3U] * m2.m[3U][2U];
    r.m[3U][3U] = this->m[3U][0U] * m2.m[0U][3U] + this->m[3U][1U] * m2.m[1U][3U] + this->m[3U][2U] * m2.m[2U][3U] + this->m[3U][3U] * m2.m[3U][3U];

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
Vector3
Matrix4::operator * (const Vector3 &v) const
{
    Vector3 r;

    float fInvW = 1.0f / (this->m[3U][0U] * v.x + this->m[3U][1U] * v.y + this->m[3U][2U] * v.z + this->m[3U][3U]);

    r.x = (this->m[0U][0U] * v.x + this->m[0U][1U] * v.y + this->m[0U][2U] * v.z + this->m[0U][3U]) * fInvW;
    r.y = (this->m[1U][0U] * v.x + this->m[1U][1U] * v.y + this->m[1U][2U] * v.z + this->m[1U][3U]) * fInvW;
    r.z = (this->m[2U][0U] * v.x + this->m[2U][1U] * v.y + this->m[2U][2U] * v.z + this->m[2U][3U]) * fInvW;

    return r;
}

//------------------------------------------------------------------------------
/**
*/
float* const
Matrix4::operator[] (unsigned int iRow) const
{
    return (float*)this->m[iRow];
}

} // namespace core
} // namespace chrissly
