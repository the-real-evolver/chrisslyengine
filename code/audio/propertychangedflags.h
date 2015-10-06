#ifndef PROPERTYCHANGEDFLAGS_H_
#define PROPERTYCHANGEDFLAGS_H_
//------------------------------------------------------------------------------
/**
    PropertyChangedFlag enumeration

    (C) 2015 Christian Bleicher
*/

//------------------------------------------------------------------------------
namespace chrissly
{
namespace audio
{
    typedef unsigned int PropertyChange;

    enum PropertyChangedFlag
    {
        UNCHANGED = 0x00000000,
        PROPERTY_PAUSED = 0x00000001,
        PROPERTY_VOLUME = 0x00000002,
        PROPERTY_PAN = 0x00000004,
        PROPERTY_MODE = 0x00000008,
        PROPERTY_ATTENUATION = 0x00000010
    };

} // namespace audio
} // namespace chrissly
//------------------------------------------------------------------------------
#endif