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
    projMatrixRS(Matrix4::ZERO)
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
const Quaternion&
Camera::GetOrientation() const
{
    return this->orientation;
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
    Vector3 zAxis = this->orientation * Vector3(0.0f, 0.0f, 1.0f);
    this->Rotate(zAxis, angle);
}

//------------------------------------------------------------------------------
/**
*/
void
Camera::Pitch(float angle)
{
    // rotate around local X axis
    Vector3 xAxis = this->orientation * Vector3(1.0f, 0.0f, 0.0f);
    this->Rotate(xAxis, angle);
}

//------------------------------------------------------------------------------
/**
*/
void
Camera::Yaw(float angle)
{
    // rotate around local y axis
    Vector3 yAxis = this->orientation * Vector3(0.0f, 1.0f, 0.0f);
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
Camera::_RenderScene(Viewport* vp)
{
    SceneManager::Instance()->_RenderScene(this, vp);
}

//------------------------------------------------------------------------------
/**
*/
const Matrix4&
Camera::GetViewMatrix() const
{
    if (this->recalcView) this->UpdateViewImpl();

    return this->viewMatrix;
}

//------------------------------------------------------------------------------
/**
*/
void
Camera::UpdateViewImpl() const
{
    // view matrix is:
    //
    //  [ Lx  Uy  Dz  Tx  ]
    //  [ Lx  Uy  Dz  Ty  ]
    //  [ Lx  Uy  Dz  Tz  ]
    //  [ 0   0   0   1   ]
    //
    // Where T = -(Transposed(Rot) * Pos)

    // this is most efficiently done using 3x3 matrices
    Matrix3 rot;
    this->orientation.ToRotationMatrix(rot);

    // Make the translation relative to new axes
    Matrix3 rotT = rot.Transpose();
    Vector3 trans = -rotT * position;

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
    
    this->recalcView = false;
}

//------------------------------------------------------------------------------
/**
*/
void
Camera::SetFOVy(const float fovy)
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
Camera::SetNearClipDistance(float nearDist)
{
    this->nearDist = nearDist;
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
Camera::SetFarClipDistance(float farDist)
{
    this->farDist = farDist;
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
Camera::GetProjectionMatrixRS() const
{
    if (this->recalcFrustum) this->UpdateFrustumImpl();

    return this->projMatrixRS;
}

//------------------------------------------------------------------------------
/**
*/
void
Camera::UpdateFrustumImpl() const
{
    // 0.5f * 3.141593f / 180.0f = 0.0087266f
    float f = 1.0f / Math::ATan(this->FOVy * 0.0087266f);
 
    this->projMatrixRS[0][0] = f / this->aspect;
    this->projMatrixRS[1][0] = 0.0f;
    this->projMatrixRS[2][0] = 0.0f; 
    this->projMatrixRS[3][0] = 0.0f;
    
    this->projMatrixRS[0][1] = 0.0f;
    this->projMatrixRS[1][1] = f;
    this->projMatrixRS[2][1] = 0.0f; 
    this->projMatrixRS[3][1] = 0.0f;
    
    this->projMatrixRS[0][2] = 0.0f;
    this->projMatrixRS[1][2] = 0.0f;
    this->projMatrixRS[2][2] = (this->farDist + this->nearDist) / (this->nearDist - this->farDist);
    this->projMatrixRS[3][2] = -1.0f;
    
    this->projMatrixRS[0][3] = 0.0f;
    this->projMatrixRS[1][3] = 0.0f;
    this->projMatrixRS[2][3] = (2.0f * this->farDist * this->nearDist) / (this->nearDist - this->farDist); 
    this->projMatrixRS[3][3] = 0.0f;
    
    this->recalcFrustum = false;
}

} // namespace graphics
} // namespace chrissly