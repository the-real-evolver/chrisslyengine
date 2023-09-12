//------------------------------------------------------------------------------
//  chrisslystring.cpp
//  (C) 2013 Christian Bleicher
//------------------------------------------------------------------------------
#include "chrisslystring.h"
#include "memoryallocatorconfig.h"
#include "debug.h"
#include <stdio.h>
#include <string.h>

namespace chrissly
{
namespace core
{

static const char* const EmptyString = "";

//------------------------------------------------------------------------------
/**
*/
String::String() :
    data(NULL),
    size(0U)
{

}

//------------------------------------------------------------------------------
/**
*/
String::String(const String& rhs) :
    data(NULL),
    size(0U)
{
    this->Set(rhs.C_Str());
}

//------------------------------------------------------------------------------
/**
*/
String::String(const char* const cStr) :
    data(NULL),
    size(0U)
{
    this->Set(cStr);
}

//------------------------------------------------------------------------------
/**
*/
String::~String()
{
    this->Delete();
}

//------------------------------------------------------------------------------
/**
*/
void
String::operator = (const String& rhs)
{
    this->Set(rhs.C_Str());
}

//------------------------------------------------------------------------------
/**
*/
void
String::operator = (const char* const cStr)
{
    this->Set(cStr);
}

//------------------------------------------------------------------------------
/**
*/
unsigned int
String::Size() const
{
    return this->size;
}

//------------------------------------------------------------------------------
/**
*/
const char* const
String::C_Str() const
{
    return this->size > 0U ? this->data : EmptyString;
}

//------------------------------------------------------------------------------
/**
*/
void
String::Set(const char* const ptr)
{
    CE_ASSERT(ptr != NULL, "String::Set(): passed character pointer not valid\n");

    this->Set(ptr, strlen(ptr));
}

//------------------------------------------------------------------------------
/**
*/
void
String::Set(const char* const ptr, unsigned int length)
{
    CE_ASSERT(ptr != NULL, "String::Set(): passed character pointer not valid\n");

    this->Delete();

    if (length > 0U)
    {
        this->data = (char*)CE_MALLOC(length + 1U);
        CE_ASSERT(this->data != NULL, "String::Set(): failed to allocate '%i' bytes\n", length + 1U);
        strncpy(this->data, ptr, length);
        this->data[length] = '\0';
        this->size = length;
    }
}

//------------------------------------------------------------------------------
/**
*/
void
String::Delete()
{
    if (this->data != NULL)
    {
        CE_FREE(this->data);
        this->data = NULL;
        this->size = 0U;
    }
}

//------------------------------------------------------------------------------
/**
*/
void
String::SubstituteString(const char* const pattern, const char* const replacement)
{
    if (NULL == pattern || NULL == replacement || this->size == 0U) return;

    size_t patternLen = strlen(pattern);
    if (0U == patternLen) return;
    char* current; char* match;
    size_t count = 0U;
    for (current = this->data; (0 != (match = strstr(current, pattern))); current = match + patternLen) ++count;
    if (0U == count) return;

    size_t replacementLen = strlen(replacement);
    size_t dstLen = (this->size - count * patternLen) + replacementLen * count;
    char* buffer = (char*)CE_MALLOC(dstLen + 1U);
    buffer[dstLen] = '\0';

    char* dst = buffer; char* src = this->data;
    while (0 != (match = strstr(src, pattern)))
    {
        size_t fragLen = match - src;
        strncpy(dst, src, fragLen);
        dst += fragLen;
        strncpy(dst, replacement, replacementLen);
        dst += replacementLen;
        src += fragLen + patternLen;
    }
    strncpy(dst, src, this->size - (src - this->data));

    this->Set(buffer);
    CE_FREE(buffer);
}

} // namespace core
} // namespace chrissly
