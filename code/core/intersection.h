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
/**
    A naive implementation of Smits ray-aabb intersection algorithm:
    https://www.realtimerendering.com/resources/RTNews/html/rtnv15n1.html#art4
*/
bool ce_intersection_ray_aabb(float ray_origin[3U], float ray_dir[3U], float aabb_min[3U], float aabb_max[3U]);
/**
    A straight forward implementation of a ray-sphere intersection (note: ray_dir has to be normalised)
*/
bool ce_intersection_ray_sphere(float ray_origin[3U], float ray_dir[3U], float sphere_center[3U], float sphere_radius);
#endif