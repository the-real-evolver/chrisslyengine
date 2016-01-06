#ifndef PIXELFORMAT_H_
#define PIXELFORMAT_H_
//------------------------------------------------------------------------------
/**
    The pixel format used for images, textures, and render surfaces

    (C) 2012 Christian Bleicher
*/

//------------------------------------------------------------------------------
namespace chrissly
{
namespace graphics
{
    enum PixelFormat
    {
        /// Unknown pixel format
        PF_UNKNOWN = 0,
        /// 16-bit pixel format, 5 bits red, 6 bits green, 5 bits blue
        PF_R5G6B5 = 1,
        /// 24-bit pixel format, 8 bits for red, green and blue
        PF_R8G8B8 = 2,
        /// 16-bit pixel format, 4 bits for alpha, red, green and blue
        PF_A4R4G4B4 = 3,
        /// 16-bit pixel format, 5 bits for blue, green, red and 1 for alpha
        PF_A1R5G5B5 = 4,
        /// 32-bit pixel format, 8 bits for red, green, blue and alpha
        PF_R8G8B8A8 = 5,
        /// 4 byte pixel format, 1 byte for red, 1 byte for green, 1 byte for blue, and one byte for alpha
        PF_BYTE_RGBA = PF_R8G8B8A8,
        /// DDS (DirectDraw Surface) DXT1 format
        PF_DXT1 = 6,
        /// DDS (DirectDraw Surface) DXT3 format
        PF_DXT3 = 7,
        /// DDS (DirectDraw Surface) DXT5 format
        PF_DXT5 = 8,
        /// Indexed, 4-bit (2 pixels per byte)
        PF_COLOR_INDEX4 = 9,
        /// Indexed, 8-bit
        PF_COLOR_INDEX8 = 10,
        /// Indexed, 16-bit
        PF_COLOR_INDEX16 = 11,
        /// Indexed, 32-bit
        PF_COLOR_INDEX32 = 12,
        /// Ericsson Texture Compression
        PF_ETC1_RGB8 = 13,
        /// Number of pixel formats currently defined
        PF_COUNT = 14
    };

} // namespace graphics
} // namespace chrissly
//------------------------------------------------------------------------------
#endif
