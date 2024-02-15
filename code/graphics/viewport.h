#ifndef VIEWPORT_H_
#define VIEWPORT_H_
//------------------------------------------------------------------------------
/**
    @class chrissly::graphics::Viewport

    (C) 2011 Christian Bleicher
*/
#include "camera.h"
#include "common.h"

//------------------------------------------------------------------------------
namespace chrissly
{
namespace graphics
{
class Camera;
class RenderTarget;

class Viewport
{
public:
    /// constructor with camera
    Viewport(Camera* const camera, RenderTarget* const target, int left, int top, int width, int height);
    /// destructor
    ~Viewport();
    /// instructs the viewport to updates its contents
    void Update();
    /// retrieves a pointer to the render target for this viewport
    RenderTarget* GetTarget() const;
    /// gets one of the actual dimensions of the viewport, a value in pixels
    int GetActualLeft() const;
    /// gets one of the actual dimensions of the viewport, a value in pixels
    int GetActualTop() const;
    /// gets one of the actual dimensions of the viewport, a value in pixels
    int GetActualWidth() const;
    /// gets one of the actual dimensions of the viewport, a value in pixels
    int GetActualHeight() const;
    /// sets the initial background colour of the viewport (before rendering)
    void SetBackgroundColour(unsigned int colour);
    /// gets the background colour
    unsigned int GetBackgroundColour() const;
    /// determines whether to clear the viewport before rendering
    void SetClearEveryFrame(bool clear, unsigned int buffers = FBT_COLOUR | FBT_DEPTH);
    /// determines if the viewport is cleared before every frame
    bool GetClearEveryFrame() const;
    /// gets which buffers are to be cleared each frame
    unsigned int GetClearBuffers() const;

private:
    Camera* camera;
    RenderTarget* target;
    int actLeft;
    int actTop;
    int actWidth;
    int actHeight;
    unsigned int backColour;
    bool clearEveryFrame;
    unsigned int clearBuffers;
};

} // namespace graphics
} // namespace chrissly
//------------------------------------------------------------------------------
#endif
