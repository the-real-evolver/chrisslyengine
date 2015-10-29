#ifndef SOUNDBASE_H_
#define SOUNDBASE_H_
//------------------------------------------------------------------------------
/**
    @class chrissly::audio::SoundBase

    (C) 2014 Christian Bleicher
*/
#include "codec.h"
#include "errorcode.h"

//------------------------------------------------------------------------------
namespace chrissly
{
namespace audio
{

class SoundBase
{
public:
    /// default constructor
    SoundBase();
    /// destructor
    virtual ~SoundBase();
    /// retrieves the length of the sound
    Result GetLength(unsigned int* length);
    /// returns format information about the sound
    Result GetFormat(SoundType* type, AudioFormat* format, int* channels, int* bits);
    /// retrieves the mode bits set by the codec and the user when opening the sound
    Result GetMode(Mode* mode);
    /// frees a sound object
    Result Release();

    /// initialize sound object
    void _Setup(const char* filename, Mode mode, Codec* codec);
    /// returns if the sound is setup
    bool _IsRealized() const;
    /// get pointer to the sample at the given position
    void* _GetSampleBufferPointer(unsigned int position) const;
    /// get pointer to the attached codec
    Codec* _GetCodec() const;
    /// implementation of creating internal sound resources
    virtual void _CreateInternalResourcesImpl();

protected:
    Mode mode;
    SoundType type;
    AudioFormat format;
    unsigned int length;
    int numChannels;
    int bitsPerSample;
    void* sampleBuffer;
    Codec* codec;
    bool realized;
};

} // namespace audio
} // namespace chrissly
//------------------------------------------------------------------------------
#endif
