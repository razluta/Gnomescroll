/* Gnomescroll, Copyright (c) 2013 Symbolic Analytics
 * Licensed under GPLv3 */
#pragma once

#include <t_map/common/types.hpp>


class RaytraceData
{
    public:
        int side;
        float interval; // % travelled
        Vec3i collision_point;

    RaytraceData() :
        side(0), interval(0.0f), collision_point(vec3i_init(0))
    {
    }

    inline CubeType get_cube_type() const;
    inline void set_collision_point(const Vec3i& position);
    Vec3i get_pre_collision_point() const;

    Vec3 collision_normal() const
    {
        return vec3_init(this->get_sides());
    }

    Vec3i get_sides() const
    {
        return get_sides_from_cube_side(this->side);
    }
};

// returns true if collided with any block
bool raytrace_terrain(const Vec3& start, const Vec3& end, class RaytraceData* data);
bool raytrace_terrain(const Vec3& start, const Vec3& direction, float length, class RaytraceData* data);

bool raytrace_terrain(const Vec3& start, const Vec3& end)
{
    return raytrace_terrain(start, end, NULL);
}

bool raytrace_terrain(const Vec3& start, const Vec3& direction, float length)
{
    return raytrace_terrain(start, direction, length, NULL);
}
