#pragma once
#if DC_SERVER

#include <agent/agent_list.hpp>
#include <game/ctf.hpp>
//#include <monsters/monsters.hpp>
#include <voxel/voxel_hitscan.hpp>

#include <particle/grenade.hpp>
//#include <particle/cspray.hpp>

namespace ServerState
{
    extern Agent_list* agent_list GNOMESCROLL_API;
        
    // property lists
    extern Voxel_hitscan_list* voxel_hitscan_list;
    //extern Grenade_shrapnel_list* grenade_shrapnel_list;

    extern CTF* ctf GNOMESCROLL_API;

    void init();
    void init_lists();
    void teardown();
    void teardown_lists();

    void damage_objects_within_sphere(
        float x, float y, float z, float radius,
        int damage, int owner,
        ObjectType inflictor_type, int inflictor_id,
        bool suicidal=true
    );

    void server_tick() GNOMESCROLL_API;

    void send_initial_game_state_to_client(int client_id);
    void send_remainining_game_state_to_client(int client_id);

    //void send_disconnect_notice(int client_id);
    void agent_disconnect(int client_id);
    void revoke_ownership(int agent_id);

    char* agent_name(int id) GNOMESCROLL_API;

    void start_game() GNOMESCROLL_API;
}

#endif
