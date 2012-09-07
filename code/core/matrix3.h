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
    float* operator[] (size_t iRow) const;
    /// negation operator
    Matrix3 operator - () const;
    /// matrix * vector [3x3 * 3x1 = 3x1]
    Vector3 operator * (const Vector3& rkPoint) const;
    /// mirrors the matrix at their main diagonal
    Matrix3 Transpose() const;

private:
    float m[3][3];
};

} // namespace core
} // namespace chrissly
//------------------------------------------------------------------------------
#endif
