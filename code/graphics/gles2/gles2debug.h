//------------------------------------------------------------------------------
/**
    @file graphics/android/gles2debug.h

    (C) 2016 Christian Bleicher
*/
#include <GLES2/gl2.h>

#define CE_GL_ENABLE_ERROR_CHECK (0)

#if CE_GL_ENABLE_ERROR_CHECK
#define CE_GL_ERROR_CHECK(context) ce_check_gl_error(context)
#else
#define CE_GL_ERROR_CHECK(context) void(context)
#endif
#define CE_PRINT_GL_STRING(name, s) ce_print_gl_string(name, s)

/// print value of the error flag the recent function call returned
void ce_check_gl_error(const char* const op);
/// print some aspect of the current GL connection
void ce_print_gl_string(const char* const name, GLenum s);
