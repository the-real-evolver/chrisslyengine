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

class Quaternion;

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
    Vector3 operator * (const float fScalar);
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
    float& operator [] (unsigned int i);
    /// access to the components by index operator
    float operator [] (unsigned int i) const;
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
    float DotProduct(const Vector3& vec) const;
    /// returns the length (magnitude) of the vector
    float Length() const;
    /// returns the square of the length (magnitude) of the vector
    float SquaredLength() const;
    /// get the maximum absolute value of the vector's components
    float GetAbsMax() const;
    /// performs a linear interpolation between two vectors, t ranges from 0 - 1
    static Vector3 Lerp(const Vector3& a, const Vector3& b, float t);
    /// gets the shortest arc quaternion to rotate this vector to the destination vector
    /**
        @remarks
            If you call this with a dest vector that is close to the inverse
            of this vector, we will rotate 180 degrees around the 'fallbackAxis'
            (if specified, or a generated axis if not) since in this case
            ANY axis of rotation is valid.
    */
    Quaternion GetRotationTo(const Vector3& dest, const Vector3& fallbackAxis = Vector3::ZERO) const;

    float x;
    float y;
    float z;

    static const Vector3 ZERO;
    static const Vector3 UNIT_POSITIVE_X;
    static const Vector3 UNIT_POSITIVE_Y;
    static const Vector3 UNIT_POSITIVE_Z;
    static const Vector3 UNIT_NEGATIVE_X;
    static const Vector3 UNIT_NEGATIVE_Y;
    static const Vector3 UNIT_NEGATIVE_Z;
    static const Vector3 UNIFORM_SCALE;
};

} // namespace core
} // namespace chrissly
//------------------------------------------------------------------------------
#endif
