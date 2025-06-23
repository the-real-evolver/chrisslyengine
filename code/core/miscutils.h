#ifndef MISCUTILS_H_
#define MISCUTILS_H_
//------------------------------------------------------------------------------
/**
    @file core/miscutils.h

    (C) 2022 Christian Bleicher
*/

/* convert argument into a string constant */
#define CE_STRINGIFY(str) #str
/* expand the argument first (if it is a macro) before stringification */
#define CE_EXPAND_AND_STRINGIFY(arg) CE_STRINGIFY(arg)

/* avoid compiler warnings about unreferenced parameters */
#define CE_UNREFERENCED_PARAMETER(P) (P)

/* get rgba components of the given colour (format: AABBGGRR) as float value */
inline void
ce_colour_convert_u32_to_float(unsigned int colour, float& red, float& green, float& blue, float& alpha)
{
    alpha = (colour >> 24U) / 255.0f;
    blue  = ((colour & 0x00ff0000) >> 16U) / 255.0f;
    green = ((colour & 0x0000ff00) >> 8U) / 255.0f;
    red   = (colour & 0x000000ff) / 255.0f;
}

#endif