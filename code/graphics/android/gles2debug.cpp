//------------------------------------------------------------------------------
//  gles2debug.cpp
//  (C) 2016 Christian Bleicher
//------------------------------------------------------------------------------
#include "gles2debug.h"
#include "debug.h"

//------------------------------------------------------------------------------
/**
*/
void
ce_check_gl_error(const char* op)
{
    GLint error;
    for (error = glGetError(); error; error = glGetError())
    {
        CE_LOG("after %s() glError (0x%x)\n", op, error);
    }
}

//------------------------------------------------------------------------------
/**
*/
void
ce_print_gl_string(const char* name, GLenum s)
{
    const char* v = (const char*)glGetString(s);
    CE_LOG("GL %s = %s\n", name, v);
}