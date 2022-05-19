#ifndef INTERSECTION_H_
#define INTERSECTION_H_
//------------------------------------------------------------------------------
/**
    @file core/intersection.h

    (C) 2022 Christian Bleicher (evolver)
*/

/**
    A batched version of the original Möller–Trumbore ray-triangle intersection
    algorithm: http://www.graphics.cornell.edu/pubs/1997/MT97.pdf
*/
bool ce_intersection_ray_triangle(const float* vertices, unsigned int num_triangles, float ray_origin[3U], float ray_dir[3U], float* const t, float* const u, float* const v, float normalOut[3U]);

#endif