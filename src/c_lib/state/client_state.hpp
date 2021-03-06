/* Gnomescroll, Copyright (c) 2013 Symbolic Analytics
 * Licensed under GPLv3 */
#pragma once

#if DC_SERVER
# error Do not include this file in the server
#endif

#include <agent/constants.hpp>
#include <entity/entities.hpp>
#include <physics/vec3.hpp>
#include <agent/client/player_agent.hpp>
#include <voxel/voxel_hitscan.hpp>
#include <voxel/voxel_render.hpp>
#include <physics/ray_trace/hitscan.hpp>

namespace ClientState
{

extern class Hitscan::WorldHitscanResult hitscan;

extern char* active_system_data_path;

extern struct Vec3i* path;
extern size_t path_len;

extern int tick_id;
extern int frame_id;

extern struct Vec3 location_pointer;
extern bool location_pointer_set;

extern class Agents::PlayerAgent player_agent;

// property lists
extern class Voxels::VoxelRenderListManager* voxel_render_list;
extern class Voxels::VoxelHitscanList* voxel_hitscan_list;

extern int last_ping_time;
extern int last_reliable_ping_time;

extern class Entities::Entity* base;

void set_player_agent_id(AgentID id);

void init_lists();
void teardown_voxel_lists();

// connect callback
void on_connect();
void on_disconnect();

void send_version();

void update_camera();
void send_camera_state();

void set_location_pointer();
void set_location_pointer_open_block();

void teardown();

void update_global_hitscan();


}   // ClientState
