#pragma once

#include <t_mech/common/common.hpp>
#include <t_mech/mech_state.hpp>

namespace t_mech
{

extern class MECH_LIST* mech_list;

void init();
void teardown();

void init_packets();

bool can_place_crystal(int x, int y, int z, int side);

void place_vine(int x, int y, int z, int side);

void tick(int x, int y, int z);

#if DC_CLIENT
void prep();
void draw();
void draw_transparent();

bool ray_cast_mech(float x, float y, float z, float vx, float vy, float vz, float* _distance);
#endif

#if DC_SERVER
void create_crystal(int x, int y, int z, int mech_type);

void send_client_mech_list(int client_id);
void handle_block_removal(int x, int y, int z); //react to block destruction
#endif

}   // t_mech
