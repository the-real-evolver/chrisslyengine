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

#endif