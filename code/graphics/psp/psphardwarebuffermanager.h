#ifndef PSPHARDWAREBUFFERMANAGER_H_
#define PSPHARDWAREBUFFERMANAGER_H_
//------------------------------------------------------------------------------
/**
    @class chrissly::PSPHardwareBufferManager

    (C) 2012 Christian Bleicher
*/

//------------------------------------------------------------------------------
namespace chrissly 
{
class PSPHardwareBufferManager
{
public:
    /// make a static allocation of vram memory and return pointer relative to vram start
    static void* GetStaticVramBuffer(unsigned int width, unsigned int height, unsigned int psm);
    /// make a static allocation of vram memory and return absolute pointer
    static void* GetStaticVramTexture(unsigned int width, unsigned int height, unsigned int psm);
    /// get the memory a buffer with the given width, height and format requires
    static unsigned int GetMemorySize(unsigned int width, unsigned int height, unsigned int psm);

private:
    static unsigned int StaticOffset;
};
} // namespace chrissly
//------------------------------------------------------------------------------
#endif
