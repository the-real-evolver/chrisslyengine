//------------------------------------------------------------------------------
//  intersection.cpp
//  (C) 2022 Christian Bleicher
//------------------------------------------------------------------------------
#include "intersection.h"

#define EPSILON 0.000001f
#define CROSS(dest,v1,v2) dest[0]=v1[1]*v2[2]-v1[2]*v2[1]; dest[1]=v1[2]*v2[0]-v1[0]*v2[2]; dest[2]=v1[0]*v2[1]-v1[1]*v2[0];
#define DOT(v1,v2) (v1[0]*v2[0]+v1[1]*v2[1]+v1[2]*v2[2])
#define SUB(dest,v1,v2) dest[0]=v1[0]-v2[0]; dest[1]=v1[1]-v2[1]; dest[2]=v1[2]-v2[2];

//------------------------------------------------------------------------------
/**
*/
bool
ce_intersection_ray_triangle(const float* vertices, unsigned int num_triangles, float ray_origin[3U], float ray_dir[3U], float* const t, float* const u, float* const v, float normalOut[3U])
{
    const float* v0; const float* v1; const float* v2;
    float edge1[3], edge2[3], tvec[3], pvec[3], qvec[3];
    float det, inv_det;

    unsigned int i;
    for (i = 0U; i < num_triangles; ++i)
    {
        v0 = vertices; vertices += 3;
        v1 = vertices; vertices += 3;
        v2 = vertices; vertices += 3;

        /* find vectors for two edges sharing v0 */
        SUB(edge1, v1, v0);
        SUB(edge2, v2, v0);

        /* begin calculating determinant - also used to calculate U parameter */
        CROSS(pvec, ray_dir, edge2);

        /* if determinant is near zero, ray lies in plane of triangle */
        det = DOT(edge1, pvec);

        if (det > -EPSILON && det < EPSILON)
            continue;
        inv_det = 1.0f / det;

        /* calculate distance from v0 to ray origin */
        SUB(tvec, ray_origin, v0);

        /* calculate U parameter and test bounds */
        *u = DOT(tvec, pvec) * inv_det;
        if (*u < 0.0f || *u > 1.0f)
            continue;

        /* prepare to test V parameter */
        CROSS(qvec, tvec, edge1);

        /* calculate V parameter and test bounds */
        *v = DOT(ray_dir, qvec) * inv_det;
        if (*v < 0.0f || *u + *v > 1.0f)
            continue;

        /* calculate t, ray intersects triangle */
        *t = DOT(edge2, qvec) * inv_det;

        /* calculate face normal */
        CROSS(normalOut, edge1, edge2);

        return true;
    }

    return false;
}