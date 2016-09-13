#ifndef VECTOR3_H_
#define VECTOR3_H_
//------------------------------------------------------------------------------
/**
    @class chrissly::core::Vector3

    (C) 2011 Christian Bleicher
*/
#include <stddef.h>

//------------------------------------------------------------------------------
namespace chrissly
{
namespace core
{

class Vector3
{
public:
    /// default constructor
    Vector3();
    /// construct from values
    Vector3(float x, float y, float z);
    /// destructor
    ~Vector3();
    /// assign operator
    Vector3& operator = (const Vector3& rkVector);
    /// vector multiplication (component product)
    Vector3 operator * (const Vector3& rhs) const;
    /// vector scalar multiplication
    Vector3& operator *= (const float fScalar);
    /// vector addition
    Vector3 operator + (const Vector3& rkVector) const;
    /// vector addition
    Vector3& operator += (const Vector3& rkVector);
    /// vector subtraction
    Vector3 operator - (const Vector3& rkVector) const;
    /// vector subtraction
    Vector3& operator -= (const Vector3& rkVector);
    /// access to the components by index operator
    float& operator [] (const size_t i);
    /// access to the components by index operator
    float operator [] (const size_t i) const;
    /// calculates the cross-product of 2 vectors, i.e. the vector that lies perpendicular to them both
    /**
        @remarks
            The cross-product is normally used to calculate the normal
            vector of a plane, by calculating the cross-product of 2
            non-equivalent vectors which lie on the plane (e.g. 2 edges
            of a triangle).
        @param
            vec Vector which, together with this one, will be used to
            calculate the cross-product.
        @returns
            A vector which is the result of the cross-product. This
            vector will <b>NOT</b> be normalised, to maximise efficiency
            - call Vector3::normalise on the result if you wish this to
            be done. As for which side the resultant vector will be on, the
            returned vector will be on the side from which the arc from 'this'
            to rkVector is anticlockwise, e.g. UNIT_Y.crossProduct(UNIT_Z)
            = UNIT_X, whilst UNIT_Z.crossProduct(UNIT_Y) = -UNIT_X.
            This is because a right-handed coordinate system is used here.
        @par
            For a clearer explanation, look a the left and the bottom edges
            of your monitor's screen. Assume that the first vector is the
            left edge and the second vector is the bottom edge, both of
            them starting from the lower-left corner of the screen. The
            resulting vector is going to be perpendicular to both of them
            and will go <i>inside</i> the screen, towards the cathode tube
            (assuming you're using a CRT monitor, of course).
    */
    Vector3 CrossProduct(const Vector3& rkVector) const;
    /// normalises the vector
    /**
        @note
            This function will not crash for zero-sized vectors, but there
            will be no changes made to their components.
    */
    void Normalise();
    /// calculates the dot (scalar) product of this vector with another
    float DotProduct(const Vector3& vec);
    /// returns the length (magnitude) of the vector
    float Length() const;

    float x;
    float y;
    float z;
};

} // namespace core
} // namespace chrissly
//------------------------------------------------------------------------------
#endif
