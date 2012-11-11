#ifndef PSPRENDERWINDOW_H_
#define PSPRENDERWINDOW_H_
//------------------------------------------------------------------------------
/**
    @class chrissly::PSPRenderWindow

    (C) 2011 Christian Bleicher
*/
#include "rendertarget.h"

//------------------------------------------------------------------------------
namespace chrissly
{
class PSPRenderWindow : public graphics::RenderTarget
{
public:
    /// default constructor
    PSPRenderWindow();
    /// destructor
    ~PSPRenderWindow();
    /// creates & displays the new window
    void Create(); 
    /// tells the window to update it's contents
    void Update();
    /// swaps the frame buffers to display the next frame
    void SwapBuffers();
};

} // namespace chrissly
//------------------------------------------------------------------------------
#endif
