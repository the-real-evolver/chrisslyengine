//------------------------------------------------------------------------------
//  vector3.cpp
//  (C) 2011 Christian Bleicher
//------------------------------------------------------------------------------
#include "vector3.h"
#include "debug.h"

namespace chrissly
{
namespace core
{

//------------------------------------------------------------------------------
/**
*/
Vector3::Vector3() : x(0.0f), y(0.0f), z(0.0f)
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
float&
Vector3::operator [] (const size_t i)
{
    CE_ASSERT(i < 3, "Vector3::operator[]: invalid index");

    return *(&this->x + i);
}

//------------------------------------------------------------------------------
/**
*/
float
Vector3::operator [] (const size_t i) const
{
    CE_ASSERT(i < 3, "Vector3::operator[]: invalid index");

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

} // namespace core
} // namespace chrissly
