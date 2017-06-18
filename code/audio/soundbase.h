#ifndef SOUNDBASE_H_
#define SOUNDBASE_H_
//------------------------------------------------------------------------------
/**
    @class chrissly::audio::SoundBase

    (C) 2014 Christian Bleicher
*/
#include "codec.h"
#include "errorcode.h"
#include "chrisslymutex.h"

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
    Result GetLength(unsigned int* const length);
    /// returns format information about the sound
    Result GetFormat(SoundType* const type, AudioFormat* const format, int* const channels, int* const bits);
    /// retrieves the mode bits set by the codec and the user when opening the sound
    Result GetMode(Mode* const modeflags);
    /// requests release
    Result Release();

    /// initialise sound object
    void _Setup(const char* const filename, Mode modeflags, Codec* const codec);
    /// internal method that frees a sound object
    void _Release();
    /// returns if the sound is setup
    bool _IsRealized() const;
    /// increments the usecount, called when a channel starts playing the sound
    void _IncrementUseCount();
    /// decrements the usecount, called when a channel using the sound was stopped
    void _DecrementUseCount();
    /// get pointer to the sample at the given position
    void* _GetSampleBufferPointer(unsigned int position) const;
    /// get pointer to the attached codec
    Codec* const  _GetCodec() const;
    /// create internal sound resources
    virtual void _CreateInternalResources();

protected:
    Mode mode;
    SoundType soundType;
    AudioFormat audioFormat;
    unsigned int lengthInSamples;
    int numChannels;
    int bitsPerSample;
    unsigned int sampleRate;
    void* sampleBuffer;
    Codec* audioCodec;
    bool realized;
    bool requestRelease;
    unsigned int useCount;
    core::Mutex releaseSyncLock;
};

} // namespace audio
} // namespace chrissly
//------------------------------------------------------------------------------
#endif
