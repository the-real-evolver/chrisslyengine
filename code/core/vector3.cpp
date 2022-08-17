//------------------------------------------------------------------------------
//  vector3.cpp
//  (C) 2011 Christian Bleicher
//------------------------------------------------------------------------------
#include "vector3.h"
#include "quaternion.h"
#include "chrisslymath.h"
#include "debug.h"

namespace chrissly
{
namespace core
{

const Vector3 Vector3::ZERO(0.0f, 0.0f, 0.0f);
const Vector3 Vector3::UNIT_POSITIVE_X(1.0f, 0.0f, 0.0f);
const Vector3 Vector3::UNIT_POSITIVE_Y(0.0f, 1.0f, 0.0f);
const Vector3 Vector3::UNIT_POSITIVE_Z(0.0f, 0.0f, 1.0f);
const Vector3 Vector3::UNIT_NEGATIVE_X(-1.0f, 0.0f, 0.0f);
const Vector3 Vector3::UNIT_NEGATIVE_Y(0.0f, -1.0f, 0.0f);
const Vector3 Vector3::UNIT_NEGATIVE_Z(0.0f, 0.0f, -1.0f);
const Vector3 Vector3::UNIFORM_SCALE(1.0f, 1.0f, 1.0f);

//------------------------------------------------------------------------------
/**
*/
Vector3::Vector3() :
    x(0.0f),
    y(0.0f),
    z(0.0f)
{

}

//------------------------------------------------------------------------------
/**
*/
Vector3::Vector3(float x, float y, float z)
{
    this->x = x;
    this->y = y;
    this->z = z;
}

//------------------------------------------------------------------------------
/**
*/
Vector3::~Vector3()
{

}

//------------------------------------------------------------------------------
/**
*/
Vector3&
Vector3::operator = (const Vector3& rkVector)
{
    this->x = rkVector.x;
    this->y = rkVector.y;
    this->z = rkVector.z;

    return *this;
}

//------------------------------------------------------------------------------
/**
*/
Vector3
Vector3::operator * (const Vector3& rhs) const
{
    return Vector3
    (
        this->x * rhs.x,
        this->y * rhs.y,
        this->z * rhs.z
    );
}

//------------------------------------------------------------------------------
/**
*/
Vector3
Vector3::operator * (const float fScalar)
{
    return Vector3
    (
        this->x * fScalar,
        this->y * fScalar,
        this->z * fScalar
    );
}

//------------------------------------------------------------------------------
/**
*/
Vector3&
Vector3::operator *= (const float fScalar)
{
    this->x *= fScalar;
    this->y *= fScalar;
    this->z *= fScalar;

    return *this;
}

//------------------------------------------------------------------------------
/**
*/
Vector3
Vector3::operator + (const Vector3& rkVector) const
{
    return Vector3
    (
        this->x + rkVector.x,
        this->y + rkVector.y,
        this->z + rkVector.z
    );
}

//------------------------------------------------------------------------------
/**
*/
Vector3&
Vector3::operator += (const Vector3& rkVector)
{
    this->x += rkVector.x;
    this->y += rkVector.y;
    this->z += rkVector.z;

    return *this;
}

//------------------------------------------------------------------------------
/**
*/
Vector3
Vector3::operator - (const Vector3& rkVector) const
{
    return Vector3
    (
        this->x - rkVector.x,
        this->y - rkVector.y,
        this->z - rkVector.z
    );
}

//------------------------------------------------------------------------------
/**
*/
Vector3&
Vector3::operator -= (const Vector3& rkVector)
{
    this->x -= rkVector.x;
    this->y -= rkVector.y;
    this->z -= rkVector.z;

    return *this;
}

//------------------------------------------------------------------------------
/**
*/
float&
Vector3::operator [] (unsigned int i)
{
    CE_ASSERT(i < 3U, "Vector3::operator[]: invalid index\n");

    return *(&this->x + i);
}

//------------------------------------------------------------------------------
/**
*/
float
Vector3::operator [] (unsigned int i) const
{
    CE_ASSERT(i < 3U, "Vector3::operator[]: invalid index\n");

    return *(&this->x + i);
}

//------------------------------------------------------------------------------
/**
*/
Vector3
Vector3::CrossProduct(const Vector3& rkVector) const
{
    return Vector3
    (
        this->y * rkVector.z - this->z * rkVector.y,
        this->z * rkVector.x - this->x * rkVector.z,
        this->x * rkVector.y - this->y * rkVector.x
    );
}

//------------------------------------------------------------------------------
/**
*/
void
Vector3::Normalise()
{
    float length = Math::Sqrt(this->x * this->x + this->y * this->y + this->z * this->z);

    if (length > 0.0f)
    {
        float invLength = 1.0f / length;
        this->x *= invLength;
        this->y *= invLength;
        this->z *= invLength;
    }
}

//------------------------------------------------------------------------------
/**
*/
float
Vector3::DotProduct(const Vector3& vec) const
{
    return this->x * vec.x + this->y * vec.y + this->z * vec.z;
}

//------------------------------------------------------------------------------
/**
*/
float
Vector3::Length() const
{
    return Math::Sqrt(this->x * this->x + this->y * this->y + this->z * this->z);
}

//------------------------------------------------------------------------------
/**
*/
float
Vector3::SquaredLength() const
{
    return this->x * this->x + this->y * this->y + this->z * this->z;
}

//------------------------------------------------------------------------------
/**
*/
float
Vector3::GetAbsMax() const
{
    float xAbs = Math::Fabs(this->x);
    float yAbs = Math::Fabs(this->y);
    float zAbs = Math::Fabs(this->z);
    float max = xAbs > yAbs ? xAbs : yAbs;
    if (zAbs > max) {return zAbs;}
    return max;
}

//------------------------------------------------------------------------------
/**
*/
Vector3
Vector3::Lerp(const Vector3& a, const Vector3& b, float t)
{
    return Vector3
    (
        ce_math_lerp(a.x, b.x, t),
        ce_math_lerp(a.y, b.y, t),
        ce_math_lerp(a.z, b.z, t)
    );
}

//------------------------------------------------------------------------------
/**
    Based on Stan Melax's article in Game Programming Gems.
*/
Quaternion
Vector3::GetRotationTo(const Vector3& dest, const Vector3& fallbackAxis) const
{
    Quaternion q;

    Vector3 v0 = *this;
    v0.Normalise();
    Vector3 v1 = dest;
    v1.Normalise();

    float d = v0.DotProduct(v1);
    // if dot == 1.0f, vectors are identical
    if (d >= 1.0f)
    {
        return Quaternion(1.0f, 0.0f, 0.0f, 0.0f);
    }
    if (d < (1e-6f - 1.0f))
    {
        if (fallbackAxis.x != 0.0f || fallbackAxis.y != 0.0f || fallbackAxis.z != 0.0f)
        {
            // rotate 180 degrees about the fallback axis
            q.FromAngleAxis((float)M_PI, fallbackAxis);
        }
        else
        {
            // generate an axis
            Vector3 axis = Vector3::UNIT_POSITIVE_X.CrossProduct(*this);
            if (axis.SquaredLength() < (1e-06 * 1e-06)) // pick another if colinear
            {
                axis = Vector3::UNIT_POSITIVE_Y.CrossProduct(*this);
            }
            axis.Normalise();
            q.FromAngleAxis((float)M_PI, axis);
        }
    }
    else
    {
        float s = Math::Sqrt((1.0f + d) * 2.0f);
        float invs = 1.0f / s;
        Vector3 c = v0.CrossProduct(v1);
        q.x = c.x * invs;
        q.y = c.y * invs;
        q.z = c.z * invs;
        q.w = s * 0.5f;
        q.Normalise();
    }

    return q;
}

} // namespace core
} // namespace chrissly
