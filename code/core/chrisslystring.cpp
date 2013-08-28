//------------------------------------------------------------------------------
//  chrisslystring.cpp
//  (C) 2011 Christian Bleicher
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
    size(0)
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

    if (length > 0)
    {
        this->data = (char*)CE_MALLOC(length + 1);
        CE_ASSERT(this->data != NULL, "String::Set(): failed to allocate '%i' bytes", length + 1);
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
        this->size = 0;
    }
}

} // namespace core
} // namespace chrissly
