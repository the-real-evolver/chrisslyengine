//------------------------------------------------------------------------------
//  dsp.cpp
//  (C) 2018 Christian Bleicher
//------------------------------------------------------------------------------
#include "dsp.h"
#include "debug.h"
#include <stdio.h>
#include <string.h>

namespace chrissly
{
namespace audio
{

//------------------------------------------------------------------------------
/**
*/
DSP::DSP() :
    release(NULL),
    process(NULL),
    setParamFloat(NULL),
    userData(NULL),
    numParameters(0),
    bypass(false),
    inUse(false)
{
    memset(this->buffer, 0, sizeof(this->buffer));
}

//------------------------------------------------------------------------------
/**
*/
DSP::~DSP()
{
    this->Release();
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
DSP::GetBypass(bool* const enabled) const
{
    *enabled = this->bypass;
    return OK;
}

//------------------------------------------------------------------------------
/**
*/

Result
DSP::GetNumParameters(int* const numParams) const
{
    *numParams = this->numParameters;
    return OK;
}

//------------------------------------------------------------------------------
/**
*/
Result
DSP::SetParameterFloat(int idx, float value)
{
    if (this->setParamFloat != NULL) return this->setParamFloat(this, idx, value);
    return OK;
}

//------------------------------------------------------------------------------
/**
*/
Result
DSP::SetUserData(void* const data)
{
    this->userData = data;
    return OK;
}

//------------------------------------------------------------------------------
/**
*/
Result
DSP::GetUserData(void** const data)
{
    *data = this->userData;
    return OK;
}

//------------------------------------------------------------------------------
/**
*/
Result
DSP::Release()
{
    Result result = OK;
    this->bypass = false;
    this->inUse = false;
    this->process = NULL;
    this->numParameters = 0;
    this->setParamFloat = NULL;
    if (this->release != NULL)
    {
        result = this->release(this);
        this->release = NULL;
    }
    this->userData = NULL;
    return result;
}

//------------------------------------------------------------------------------
/**
*/
void
DSP::Setup(const DspDescription* const desc)
{
    CE_ASSERT(desc->process != NULL, "DSP::Setup(): pointer to Process() callback must not be NULL\n");
    this->bypass = false;
    this->inUse = true;
    this->release = desc->release;
    this->process = desc->process;
    this->numParameters = desc->numParameters;
    this->setParamFloat = desc->setParamFloat;
    this->userData = desc->userData;
    if (desc->setup != NULL) (void)desc->setup(this);
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