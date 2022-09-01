#ifndef SKELETONSERIALISER_H_
#define SKELETONSERIALISER_H_
//------------------------------------------------------------------------------
/**
    @file graphics/skeletonserialiser.h

    (C) 2022 Christian Bleicher
*/
#include "mesh.h"

/// import skeleton and animations and attach them to the given mesh
void ce_graphics_import_skeleton(char const* const file_path, chrissly::graphics::Mesh* const mesh);

#endif
