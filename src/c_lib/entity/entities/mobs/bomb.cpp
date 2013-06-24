#include "bomb.hpp"

#include <entity/entity/entity.hpp>
#include <entity/constants.hpp>
#include <entity/components/health.hpp>
#include <entity/components/dimension.hpp>
#include <entity/components/voxel_model.hpp>
#if DC_SERVER
# include <entity/components/explosion.hpp>
# include <entity/entities/mobs/state_machines.hpp>
#endif

namespace Entities
{

static void bomb_state_router(class Entity*, EntityState state);

void load_mob_bomb_data()
{
    EntityType type = ENTITY_MONSTER_BOMB;

    entity_data->begin_attaching_to(type);

    ADD_COMPONENT(PositionMomentum);

    auto dims = ADD_COMPONENT(Dimension);
    dims->height = 1.0f;

    auto vox = ADD_COMPONENT(VoxelModel);
    vox->vox_dat = &VoxDats::robot_bomb;
    vox->init_hitscan = true;
    vox->init_draw = true;

    #if DC_CLIENT
    ADD_COMPONENT(HitPoints);
    #endif
    #if DC_SERVER   // health will be set by packet initializer in client, so dont initialize it here
    auto health = ADD_COMPONENT(HitPoints);
    health->health = 80;
    health->health_max = 80;
    #endif

    auto dest = ADD_COMPONENT(DestinationTargeting);
    dest->sight_range = 15.0f;
    dest->destination_choice_x = 20;
    dest->destination_choice_y = 20;
    dest->speed = 0.125f;
    dest->max_z_diff = 5;

    auto agent = ADD_COMPONENT(AgentTargeting);
    agent->sight_range = 15.0f;
    agent->speed = 0.25f;
    agent->max_z_diff = 5;
    agent->max_lock_ticks = ONE_SECOND * 10;

    auto waiting = ADD_COMPONENT(Waiting);
    waiting->wait_time = ONE_SECOND * 3;

    #if DC_SERVER
    auto explode = ADD_COMPONENT(Explosion);
    explode->radius = 4.0f;
    explode->proximity_radius = 0.8f;
    explode->damage = 40;
    explode->block_destruction_radius = 3.0f;
    explode->block_damage = 16;
    explode->terrain_modification_action = TMA_MONSTER_BOMB;
    explode->delay = MOB_BROADCAST_RATE;

    auto limiter = ADD_COMPONENT(RateLimit);
    limiter->limit = MOB_BROADCAST_RATE;

    auto item_drop = ADD_COMPONENT(ItemDrop);
    item_drop->drop->set_max_drop_types(2);
    item_drop->drop->set_max_drop_amounts("synthesizer_coin", 3);
    item_drop->drop->add_drop("synthesizer_coin", 1, 0.3f);
    item_drop->drop->add_drop("synthesizer_coin", 2, 0.1f);
    item_drop->drop->add_drop("synthesizer_coin", 3, 0.05f);

    item_drop->drop->set_max_drop_amounts("plasma_grenade", 10);
    item_drop->drop->add_drop_range("plasma_grenade", 1, 10, 0.8f);

    auto state = ADD_COMPONENT(StateMachine);
    state->state = STATE_WAITING;
    state->router = &bomb_state_router;

    auto knockback = ADD_COMPONENT(Knockback);
    knockback->weight = 1.0f;
    #endif

    #if DC_CLIENT
    auto anim = ADD_COMPONENT(Animation);
    anim->color = Color(31, 223, 233);
    anim->count = 50;
    anim->count_max = 100;
    anim->size = 0.7f;
    anim->force = 20.0f;
    #endif
}

void ready_mob_bomb(Entity* entity)
{
    auto vox = GET_COMPONENT_INTERFACE(VoxelModel, entity);
    auto physics = GET_COMPONENT_INTERFACE(Physics, entity);

    Vec3 position = physics->get_position();
    Vec3 angles = physics->get_angles();

    vox->ready(position, angles.x, angles.y);

    #if DC_SERVER
    entity->broadcastCreate();
    #endif
}

void die_mob_bomb(Entity* entity)
{
    #if DC_SERVER
    // drop item
    auto item_drop = GET_COMPONENT_INTERFACE(ItemDrop, entity);
    GS_ASSERT(item_drop != NULL);
    item_drop->drop_item();

    // explosion damage
    auto explode = GET_COMPONENT_INTERFACE(Explosion, entity);
    explode->explode();
    explode->damage_blocks();

    // notify clients
    entity->broadcastDeath();
    #endif

    #if DC_CLIENT
    // explosion animation
    auto vox = GET_COMPONENT_INTERFACE(VoxelModel, entity);
    if (vox->vox != NULL)
    {
        auto anim = GET_COMPONENT_INTERFACE(Animation, entity);
        anim->explode_random(vox->get_center());
    }
    #endif
}

#if DC_SERVER
static void bomb_state_router(class Entity* entity, EntityState state)
{
    auto machine = GET_COMPONENT_INTERFACE(StateMachine, entity);

    switch (state)
    {
        case STATE_CHASE_AGENT:
            if (machine->state == STATE_WAITING)
                waiting_to_chase_agent(entity);
            else if (machine->state == STATE_IN_TRANSIT)
                in_transit_to_chase_agent(entity);
            break;

        case STATE_IN_TRANSIT:
            if (machine->state == STATE_WAITING)
                waiting_to_in_transit(entity);
            else if (machine->state == STATE_CHASE_AGENT)
                chase_agent_to_in_transit(entity);
            break;

        case STATE_WAITING:
            if (machine->state == STATE_CHASE_AGENT)
                chase_agent_to_waiting(entity);
            else if (machine->state == STATE_IN_TRANSIT)
                in_transit_to_waiting(entity);
            break;

        case STATE_NONE:
            GS_ASSERT(false);
            break;
    }
}
#endif

void tick_mob_bomb(Entity* entity)
{
    #if DC_SERVER
     //die if near agent
    auto explode = GET_COMPONENT_INTERFACE(Explosion, entity);
    explode->proximity_check();

    auto limiter = GET_COMPONENT_INTERFACE(RateLimit, entity);
    if (limiter->allowed()) entity->broadcastState();

    auto machine = GET_COMPONENT_INTERFACE(StateMachine, entity);

    switch (machine->state)
    {
        case STATE_WAITING:
            waiting(entity);
            break;

        case STATE_IN_TRANSIT:
            in_transit(entity);
            break;

        case STATE_CHASE_AGENT:
            chase_agent(entity);
            break;

        case STATE_NONE:
            GS_ASSERT(false);
            break;
    }

    if (machine->state != STATE_CHASE_AGENT)
    {   // aggro nearby agent
        auto physics = GET_COMPONENT_INTERFACE(Physics, entity);
        Vec3 position = physics->get_position();

        auto target = GET_COMPONENT(AgentTargeting, entity);
        target->lock_target(position);

        if (target->target_type == ENTITY_AGENT)
            machine->router(entity, STATE_CHASE_AGENT);
    }
    #endif
}

void update_mob_bomb(Entity* entity)
{
    auto physics = GET_COMPONENT_INTERFACE(Physics, entity);
    auto vox = GET_COMPONENT_INTERFACE(VoxelModel, entity);

    Vec3 angles = physics->get_angles();
    vox->update(physics->get_position(), angles.x, angles.y, physics->get_changed());
    physics->set_changed(false);  // reset changed state
}

} // Entities
