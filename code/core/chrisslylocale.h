#ifndef CHRISSLYLOCALE_H_
#define CHRISSLYLOCALE_H_
//------------------------------------------------------------------------------
/**
    @file core/chrisslylocale.h

    Provides functionality for localising strings to different languages. 

    (C) 2022 Christian Bleicher
*/

/// Macro for localizing the string s. Should only be used for static in-place strings, e.g. CE_LOCALISE("name"). 
#define CE_LOCALISE(s) s

#endif