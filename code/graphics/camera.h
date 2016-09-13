#ifndef CAMERA_H_
#define CAMERA_H_
//------------------------------------------------------------------------------
/**
    @class chrissly::graphics::Camera

    (C) 2011 Christian Bleicher
*/
#include "viewport.h"
#include "quaternion.h"
#include "vector3.h"
#include "matrix4.h"

//------------------------------------------------------------------------------
namespace chrissly
{
namespace graphics
{
class Viewport;

class Camera
{
public:
    /// default constructor
    Camera();
    /// destructor
    ~Camera();
    /// sets the camera's position
    void SetPosition(float x, float y, float z);
    /// sets the camera's position
    void SetPosition(const core::Vector3& vec);
    /// Retrieves the camera's position.
    const core::Vector3& GetPosition() const;
     /// sets the camera's orientation
    void SetOrientation(const core::Quaternion& q);
    /// returns the camera's current orientation
    const core::Quaternion& GetOrientation() const;
    /// gets the camera's direction
    core::Vector3 GetDirection() const;
    /// gets the camera's up vector
    core::Vector3 GetUp() const;
    /// moves the camera's position by the vector offset provided along it's own axes (relative to orientation)
    void MoveRelative(const core::Vector3& vec);
    /// rolls the camera anticlockwise, around its local z axis
    void Roll(float angle);
    /// pitches the camera up/down anticlockwise around it's local x axis
    void Pitch(float angle);
    /// rotates the camera anticlockwise around it's local y axis
    void Yaw(float angle);
    /// rotate the camera around an arbitrary axis
    void Rotate(const core::Vector3& axis, float angle);
    /// rotate the camera around an arbitrary axis using a Quaternion
    void Rotate(const core::Quaternion& q);
    /// sets the Y-dimension Field Of View (FOV) of the frustum
    void SetFOVy(const float fovy);
    /// retrieves the frustums Y-dimension Field Of View (FOV)
    float GetFOVy() const;
    /// sets the position of the near clipping plane
    void SetNearClipDistance(float dist);
    /// gets the position of the near clipping plane
    float GetNearClipDistance() const;
    /// sets the distance to the far clipping plane
    void SetFarClipDistance(float dist);
    /// retrieves the distance from the frustum to the far clipping plane
    float GetFarClipDistance() const;
    /// sets the aspect ratio for the frustum viewport
    void SetAspectRatio(float ratio);
    /// retreives the current aspect ratio.
    float GetAspectRatio() const;
    /// gets the view matrix for this frustum. Mainly for use by engine internally
    const core::Matrix4& GetViewMatrix() const;
    /// gets the projection matrix for this frustum adjusted for the rendersystem
    const core::Matrix4& GetProjectionMatrixRS() const;
    /// tells the Camera to contact the SceneManager to render from it's viewpoint
    void _RenderScene(Viewport* vp);

private:
    /// implementation of updateView (called if out of date)
    void UpdateView() const;
    /// implementation of updateFrustum (called if out of date) 
    void UpdateFrustum() const;
    /// camera orientation, quaternion style
    core::Quaternion orientation;
    /// camera position - default (0,0,0)
    core::Vector3 position;
    /// pre-calced view matrix
    mutable core::Matrix4 viewMatrix;
    /// something changed the viewmatrix
    mutable bool recalcView;
    /// something's changed in the frustum shape
    mutable bool recalcFrustum;
    /// y-direction field-of-view (default 90)
    float FOVy;
    /// far clip distance - default 1000
    float farDist;
    /// near clip distance - default 0.1
    float nearDist;
    /// x/y viewport ratio - default 1.7777
    float aspect;
    /// pre-calced projection matrix for the psp rendersystem
    mutable core::Matrix4 projMatrixRS;
};

} // namespace graphics
} // namespace chrissly
//------------------------------------------------------------------------------
#endif
