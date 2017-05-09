//------------------------------------------------------------------------------
//  plane.cpp
//  (C) 2017 Christian Bleicher
//------------------------------------------------------------------------------
#include "plane.h"

namespace chrissly
{
namespace core
{

//------------------------------------------------------------------------------
/**
*/
Plane::Plane() :
    d(0.0f)
{

}

//------------------------------------------------------------------------------
/**
*/
Plane::~Plane()
{

}

//------------------------------------------------------------------------------
/**
*/
void
Plane::FromPoints(const Vector3& point0, const Vector3& point1, const Vector3& point2)
{
    Vector3 edge0 = point0 - point1;
    Vector3 edge1 = point2 - point1;
    this->normal = edge1.CrossProduct(edge0);
    this->normal.Normalise();
    this->d = -(this->normal.DotProduct(point1));
}

//------------------------------------------------------------------------------
/**
*/
float
Plane::Distance(const Vector3& point) const
{
    return this->d + this->normal.DotProduct(point);
}

} // namespace core
} // namespace chrissly
