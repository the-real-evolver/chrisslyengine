#ifndef MATRIX4_H_
#define MATRIX4_H_
//------------------------------------------------------------------------------
/**
    @class chrissly::core::Matrix4

    (C) 2012 Christian Bleicher
*/
#include <stddef.h>

//------------------------------------------------------------------------------
namespace chrissly
{
namespace core
{

class Matrix4
{
public:
    /** default constructor
        @note
            It does <b>NOT</b> initialize the matrix for efficiency.
    */
    Matrix4();
    /// constructor with values
    Matrix4(float m00, float m01, float m02, float m03,
            float m10, float m11, float m12, float m13,
            float m20, float m21, float m22, float m23,
            float m30, float m31, float m32, float m33);
    /// destructor
    ~Matrix4();
    /// concatenate with the given matrix
    Matrix4 Concatenate(const Matrix4& m2) const;
    /// matrix concatenation using '*'
    Matrix4 operator * (const Matrix4 &m2) const;
    /// member access, allows use of construct mat[r][c]
    float* operator[] (size_t iRow) const;
    /// identity matrix
    static const Matrix4 IDENTITY;
    /// zero initialized matrix
    static const Matrix4 ZERO;

private:
    float m[4][4];
};

} // namespace core
} // namespace chrissly
//------------------------------------------------------------------------------
#endif
