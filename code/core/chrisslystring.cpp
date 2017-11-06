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
String::Size()
{
    return this->size;
}

//------------------------------------------------------------------------------
/**
*/
const char* const
String::C_Str() const
{
    return this->data;
}

//------------------------------------------------------------------------------
/**
*/
void
String::Set(const char* const ptr)
{
    this->Set(ptr, strlen(ptr));
}

//------------------------------------------------------------------------------
/**
*/
void
String::Set(const char* const ptr, unsigned int length)
{
    this->Delete();

    if (length > 0U)
    {
        this->data = (char*)CE_MALLOC(length + 1U);
        CE_ASSERT(this->data != NULL, "String::Set(): failed to allocate '%i' bytes", length + 1U);
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

} // namespace core
} // namespace chrissly
