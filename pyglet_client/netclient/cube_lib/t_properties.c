#include "./t_properties.h"

#define max_cubes 1024
struct cubeProperties cube_list[max_cubes];

struct cubeProperties* _get_cube_list() {
    return &cube_list;
}

struct cubeProperties* _get_cube(int id) {
    return &cube_list[id];
}
