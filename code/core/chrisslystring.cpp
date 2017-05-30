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
String::String(const String& rhs)
{
    this->Set(rhs.C_Str());
}

//------------------------------------------------------------------------------
/**
*/
String::String(const char* cStr)
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
String::operator=(const String& rhs)
{
    this->Set(rhs.C_Str());
}

//------------------------------------------------------------------------------
/**
*/
void
String::operator=(const char* cStr)
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
const char*
String::C_Str() const
{
    return this->data;
}

//------------------------------------------------------------------------------
/**
*/
void
String::Set(const char* ptr)
{
    size_t length = strlen(ptr);
    this->Set(ptr, length);
}

//------------------------------------------------------------------------------
/**
*/
void
String::Set(const char* ptr, unsigned int length)
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
    if (NULL != this->data)
    {
        CE_FREE(this->data);
        this->data = NULL;
        this->size = 0U;
    }
}

} // namespace core
} // namespace chrissly
