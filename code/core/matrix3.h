#ifndef MATRIX3_H_
#define MATRIX3_H_
//------------------------------------------------------------------------------
/**
    @class chrissly::core::Matrix3

    (C) 2011 Christian Bleicher
*/
#include "vector3.h"
#include <stddef.h>

//------------------------------------------------------------------------------
namespace chrissly
{
namespace core
{

class Matrix3
{
    friend class Quaternion;
public:
    /// default constructor
    Matrix3();
    /// destructor
    ~Matrix3();
    /// member access, allows use of construct mat[r][c]
    float* operator[] (unsigned int iRow) const;
    /// negation operator
    Matrix3 operator - () const;
    /// matrix concatenation using '*'
    Matrix3 operator * (const Matrix3& m2) const;
    /// matrix * vector [3x3 * 3x1 = 3x1]
    Vector3 operator * (const Vector3& rkPoint) const;
    /// mirrors the matrix at their main diagonal
    Matrix3 Transpose() const;
    /// decompose matrix into yaw-pitch-roll angles
    bool ToEulerAngles(float& yaw, float& pitch, float& roll) const;

private:
    float m[3U][3U];
};

} // namespace core
} // namespace chrissly
//------------------------------------------------------------------------------
#endif
