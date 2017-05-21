//------------------------------------------------------------------------------
//  camera.cpp
//  (C) 2011 Christian Bleicher
//------------------------------------------------------------------------------
#include "camera.h"
#include "scenemanager.h"
#include "chrisslymath.h"

namespace chrissly
{
namespace graphics
{

using namespace chrissly::core;

//------------------------------------------------------------------------------
/**
*/
Camera::Camera() :
    viewMatrix(Matrix4::IDENTITY),
    recalcView(true),
    recalcFrustum(true),
    FOVy(90.0f),
    farDist(1000.0f),
    nearDist(0.1f),
    aspect(1.7777f),
    projMatrix(Matrix4::ZERO),
    nearHeight(0.0f),
    nearWidth(0.0f),
    farHeight(0.0f),
    farWidth(0.0f)
{

}

//------------------------------------------------------------------------------
/**
*/
Camera::~Camera()
{

}

//------------------------------------------------------------------------------
/**
*/
void
Camera::SetPosition(float x, float y, float z)
{
    this->position.x = x;
    this->position.y = y;
    this->position.z = z;
    this->recalcView = true;
}

//------------------------------------------------------------------------------
/**
*/
void
Camera::SetPosition(const Vector3& vec)
{
    this->position = vec;
    this->recalcView = true;
}

//------------------------------------------------------------------------------
/**
*/
const Vector3&
Camera::GetPosition() const
{
    return this->position;
}

//------------------------------------------------------------------------------
/**
*/
void
Camera::SetOrientation(const Quaternion& q)
{
    this->orientation = q;
    this->orientation.Normalise();
    this->recalcView = true;
}

//------------------------------------------------------------------------------
/**
*/
const Quaternion&
Camera::GetOrientation() const
{
    return this->orientation;
}

//------------------------------------------------------------------------------
/**
*/
void
Camera::SetDirection(const Vector3& direction)
{
    Vector3 yawFixedAxis = Vector3::UNIT_POSITIVE_Y;
    Vector3 zAxis = direction;
    zAxis *= -1.0f;
    zAxis.Normalise();
    Vector3 xAxis = yawFixedAxis.CrossProduct(zAxis);
    xAxis.Normalise();
    Vector3 yAxis = zAxis.CrossProduct(xAxis);
    yAxis.Normalise();
    this->orientation.FromAxes(xAxis, yAxis, zAxis);
}

//------------------------------------------------------------------------------
/**
*/
Vector3
Camera::GetDirection() const
{
    return this->orientation * Vector3::UNIT_NEGATIVE_Z;
}

//------------------------------------------------------------------------------
/**
*/
Vector3
Camera::GetUp() const
{
    return this->orientation * Vector3::UNIT_POSITIVE_Y;
}

//------------------------------------------------------------------------------
/**
*/
void
Camera::MoveRelative(const Vector3& vec)
{
    // transform the axes of the relative vector by camera's local axes
    Vector3 trans = this->orientation * vec;
    this->position = this->position + trans;
    this->recalcView = true; 
}

//------------------------------------------------------------------------------
/**
*/
void
Camera::Roll(float angle)
{
    // rotate around local Z axis
    Vector3 zAxis = this->orientation * Vector3::UNIT_POSITIVE_Z;
    this->Rotate(zAxis, angle);
}

//------------------------------------------------------------------------------
/**
*/
void
Camera::Pitch(float angle)
{
    // rotate around local X axis
    Vector3 xAxis = this->orientation * Vector3::UNIT_POSITIVE_X;
    this->Rotate(xAxis, angle);
}

//------------------------------------------------------------------------------
/**
*/
void
Camera::Yaw(float angle)
{
    // rotate around local y axis
    Vector3 yAxis = this->orientation * Vector3::UNIT_POSITIVE_Y;
    this->Rotate(yAxis, angle);
}

//------------------------------------------------------------------------------
/**
*/
void
Camera::Rotate(const Vector3& axis, float angle)
{
    Quaternion q;
    q.FromAngleAxis(angle, axis);
    this->Rotate(q);
}

//------------------------------------------------------------------------------
/**
*/
void
Camera::Rotate(const Quaternion& q)
{
    // note the order of the mult, i.e. q comes after
    // normalise the quat to avoid cumulative problems with precision
    Quaternion qnorm = q;
    qnorm.Normalise();
    this->orientation = qnorm * this->orientation;
    this->recalcView = true;
}

//------------------------------------------------------------------------------
/**
*/
void
Camera::SetFOVy(float fovy)
{
    this->FOVy = fovy;
    this->recalcFrustum = true;
}

//------------------------------------------------------------------------------
/**
*/
float
Camera::GetFOVy() const
{
    return this->FOVy;
}

//------------------------------------------------------------------------------
/**
*/
void
Camera::SetNearClipDistance(float dist)
{
    this->nearDist = dist;
    this->recalcFrustum = true;
}

//------------------------------------------------------------------------------
/**
*/
float
Camera::GetNearClipDistance() const
{
    return this->nearDist;
}

//------------------------------------------------------------------------------
/**
*/
void
Camera::SetFarClipDistance(float dist)
{
    this->farDist = dist;
    this->recalcFrustum = true;
}

//------------------------------------------------------------------------------
/**
*/
float
Camera::GetFarClipDistance() const
{
    return this->farDist;
}

//------------------------------------------------------------------------------
/**
*/
void
Camera::SetAspectRatio(float ratio)
{
    this->aspect = ratio;
    this->recalcFrustum = true;
}

//------------------------------------------------------------------------------
/**
*/
float
Camera::GetAspectRatio() const
{
    return this->aspect;
}

//------------------------------------------------------------------------------
/**
*/
const Matrix4&
Camera::GetViewMatrix() const
{
    if (this->recalcView)
    {
        this->UpdateView();
    }

    return this->viewMatrix;
}

//------------------------------------------------------------------------------
/**
*/
const Matrix4&
Camera::GetProjectionMatrix() const
{
    if (this->recalcFrustum)
    {
        this->UpdateFrustum();
    }

    return this->projMatrix;
}

//------------------------------------------------------------------------------
/**
*/
bool
Camera::IsVisible(const Vector3& center, float radius) const
{
    if (this->recalcFrustum)
    {
        this->UpdateFrustum();
    }
    if (this->recalcView)
    {
        this->UpdateView();
    }

    float distance = 0.0f;
    unsigned int i;
    for (i = 0; i < FRUSTUM_PLANE_COUNT; ++i)
    {
        distance = this->frustumPlanes[i].Distance(center);
        if (distance < -radius)
        {
            return false;
        }
    }

    return true;
}

//------------------------------------------------------------------------------
/**
*/
void
Camera::_RenderScene(Viewport* vp)
{
    SceneManager::Instance()->_RenderScene(this, vp);
}

//------------------------------------------------------------------------------
/**
*/
void
Camera::UpdateView() const
{
    // view matrix is:
    //
    //  [ Lx  Uy  Dz  Tx ]
    //  [ Lx  Uy  Dz  Ty ]
    //  [ Lx  Uy  Dz  Tz ]
    //  [ 0   0   0   1  ]
    //
    // Where T = -(Transposed(Rot) * Pos)

    // this is most efficiently done using 3x3 matrices
    Matrix3 rot;
    this->orientation.ToRotationMatrix(rot);

    // Make the translation relative to new axes
    Matrix3 rotT = rot.Transpose();
    Vector3 trans = -rotT * this->position;

    // make final matrix
    // fills upper 3x3
    this->viewMatrix[0][0] = rotT[0][0];
    this->viewMatrix[0][1] = rotT[0][1];
    this->viewMatrix[0][2] = rotT[0][2];
    this->viewMatrix[1][0] = rotT[1][0];
    this->viewMatrix[1][1] = rotT[1][1];
    this->viewMatrix[1][2] = rotT[1][2];
    this->viewMatrix[2][0] = rotT[2][0];
    this->viewMatrix[2][1] = rotT[2][1];
    this->viewMatrix[2][2] = rotT[2][2];

    this->viewMatrix[0][3] = trans.x;
    this->viewMatrix[1][3] = trans.y;
    this->viewMatrix[2][3] = trans.z;

    // update frustum planes
    Vector3 xAxis = this->orientation * Vector3::UNIT_POSITIVE_X;
    xAxis.Normalise();
    Vector3 yAxis = this->orientation * Vector3::UNIT_POSITIVE_Y;
    yAxis.Normalise();
    Vector3 zAxis = this->orientation * Vector3::UNIT_POSITIVE_Z;
    zAxis.Normalise();

    Vector3 nearCenter = this->position - zAxis * this->nearDist;
    Vector3 farCenter = this->position - zAxis * this->farDist;

    Vector3 nearTopLeft = nearCenter + yAxis * this->nearHeight - xAxis * this->nearWidth;
    Vector3 nearTopRight = nearCenter + yAxis * this->nearHeight + xAxis * this->nearWidth;
    Vector3 nearBottomLeft = nearCenter - yAxis * this->nearHeight - xAxis * this->nearWidth;
    Vector3 nearBottomRight = nearCenter - yAxis * this->nearHeight + xAxis * this->nearWidth;

    Vector3 farTopLeft = farCenter + yAxis * this->farHeight - xAxis * this->farWidth;
    Vector3 farTopRight = farCenter + yAxis * this->farHeight + xAxis * this->farWidth;
    Vector3 farBottomLeft = farCenter - yAxis * this->farHeight - xAxis * this->farWidth;
    Vector3 farBottomRight = farCenter - yAxis * this->farHeight + xAxis * this->farWidth;

    this->frustumPlanes[FRUSTUM_PLANE_TOP].FromPoints(nearTopRight, nearTopLeft, farTopLeft);
    this->frustumPlanes[FRUSTUM_PLANE_BOTTOM].FromPoints(nearBottomLeft, nearBottomRight, farBottomRight);
    this->frustumPlanes[FRUSTUM_PLANE_LEFT].FromPoints(nearTopLeft, nearBottomLeft, farBottomLeft);
    this->frustumPlanes[FRUSTUM_PLANE_RIGHT].FromPoints(nearBottomRight, nearTopRight, farBottomRight);
    this->frustumPlanes[FRUSTUM_PLANE_NEAR].FromPoints(nearTopLeft, nearTopRight, nearBottomRight);
    this->frustumPlanes[FRUSTUM_PLANE_FAR].FromPoints(farTopRight, farTopLeft, farBottomLeft);

    this->recalcView = false;
}

//------------------------------------------------------------------------------
/**
*/
void
Camera::UpdateFrustum() const
{
    static const float DegreeToRadianHalf = 3.141593f / 180.0f * 0.5f;

    float fovy = this->FOVy * DegreeToRadianHalf;
    float f = 1.0f / Math::ATan(fovy);

    this->projMatrix[0][0] = f / this->aspect;
    this->projMatrix[1][0] = 0.0f;
    this->projMatrix[2][0] = 0.0f;
    this->projMatrix[3][0] = 0.0f;

    this->projMatrix[0][1] = 0.0f;
    this->projMatrix[1][1] = f;
    this->projMatrix[2][1] = 0.0f;
    this->projMatrix[3][1] = 0.0f;

    this->projMatrix[0][2] = 0.0f;
    this->projMatrix[1][2] = 0.0f;
    this->projMatrix[2][2] = (this->farDist + this->nearDist) / (this->nearDist - this->farDist);
    this->projMatrix[3][2] = -1.0f;

    this->projMatrix[0][3] = 0.0f;
    this->projMatrix[1][3] = 0.0f;
    this->projMatrix[2][3] = (2.0f * this->farDist * this->nearDist) / (this->nearDist - this->farDist);
    this->projMatrix[3][3] = 0.0f;

    float tangent = Math::Tan(fovy);
    this->nearHeight = this->nearDist * tangent;
    this->nearWidth = this->nearHeight * this->aspect;
    this->farHeight = this->farDist * tangent;
    this->farWidth = this->farHeight * this->aspect;

    this->recalcFrustum = false;
}

} // namespace graphics
} // namespace chrissly