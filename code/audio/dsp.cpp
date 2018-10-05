//------------------------------------------------------------------------------
//  dsp.cpp
//  (C) 2018 Christian Bleicher
//------------------------------------------------------------------------------
#include "dsp.h"
#include <stdio.h>

namespace chrissly
{
namespace audio
{

//------------------------------------------------------------------------------
/**
*/
DSP::DSP() :
    bypass(false),
    inUse(false),
    Process(NULL)
{

}

//------------------------------------------------------------------------------
/**
*/
DSP::~DSP()
{

}

//------------------------------------------------------------------------------
/**
*/
Result
DSP::SetBypass(bool enabled)
{
    this->bypass = enabled;
    return OK;
}

//------------------------------------------------------------------------------
/**
*/
Result
DSP::GetBypass(bool* const enabled)
{
    *enabled = this->bypass;
    return OK;
}

//------------------------------------------------------------------------------
/**
*/
Result
DSP::Release()
{
    this->bypass = false;
    this->inUse = false;
    this->Process = NULL;
    return OK;
}

//------------------------------------------------------------------------------
/**
*/
void
DSP::Setup(const DspDescription* const desc)
{
    this->bypass = false;
    this->inUse = true;
    this->Process = desc->Process;
}

//------------------------------------------------------------------------------
/**
*/
bool
DSP::IsInUse() const
{
    return this->inUse;
}

} // namespace audio
} // namespace chrissly