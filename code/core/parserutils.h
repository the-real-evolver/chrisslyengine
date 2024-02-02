//------------------------------------------------------------------------------
/**
    @file core/parserutils.h

    Convenience functions for parsing signed numbers with stb_c_lexer.

    (C) 2024 Christian Bleicher
*/
#include "stb_c_lexer.h"
#include "matrix4.h"

/* parse integer number considering its sign */
inline int
ce_parse_signed_int(stb_lexer* const lexer)
{
    if (0 == stb_c_lexer_get_token(lexer)) {return 0;}
    if ('-' == lexer->token)
    {
        if (0 == stb_c_lexer_get_token(lexer)) {return 0;}
        return -(int)lexer->int_number;
    }
    return (int)lexer->int_number;
}

/* parse floating point number considering its sign */
inline float
ce_parse_float(stb_lexer* const lexer)
{
    if (0 == stb_c_lexer_get_token(lexer)) {return 0.0f;}
    if ('-' == lexer->token)
    {
        if (0 == stb_c_lexer_get_token(lexer)) {return 0.0f;}
        return -(float)lexer->real_number;
    }
    return (float)lexer->real_number;
}

/* parse a row of floating point numbers and create a 4x4 matrix from them */
inline chrissly::core::Matrix4
ce_parse_matrix4(stb_lexer* const lexer)
{
    chrissly::core::Matrix4 m;
    m[0U][0U] = ce_parse_float(lexer); m[0U][1U] = ce_parse_float(lexer); m[0U][2U] = ce_parse_float(lexer); m[0U][3U] = ce_parse_float(lexer);
    m[1U][0U] = ce_parse_float(lexer); m[1U][1U] = ce_parse_float(lexer); m[1U][2U] = ce_parse_float(lexer); m[1U][3U] = ce_parse_float(lexer);
    m[2U][0U] = ce_parse_float(lexer); m[2U][1U] = ce_parse_float(lexer); m[2U][2U] = ce_parse_float(lexer); m[2U][3U] = ce_parse_float(lexer);
    m[3U][0U] = ce_parse_float(lexer); m[3U][1U] = ce_parse_float(lexer); m[3U][2U] = ce_parse_float(lexer); m[3U][3U] = ce_parse_float(lexer);
    return m;
}
