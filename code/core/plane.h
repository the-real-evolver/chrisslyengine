#ifndef PLANE_H_
#define PLANE_H_
//------------------------------------------------------------------------------
/**
    @class chrissly::core::Plane

    (C) 2017 Christian Bleicher
*/
#include "vector3.h"

//------------------------------------------------------------------------------
namespace chrissly
{
namespace core
{

class Plane
{
public:
    /// default constructor
    Plane();
    /// destructor
    ~Plane();
    /// updates the plane from three points (assumes that the points are given in counter clockwise order)
    void FromPoints(const Vector3& point0, const Vector3& point1, const Vector3& point2);
    /// returns the distance of the given point to the plane
    float Distance(const Vector3& point) const;

private:
    Vector3 normal;
    float d;
};

} // namespace core
} // namespace chrissly
//------------------------------------------------------------------------------
#endif
