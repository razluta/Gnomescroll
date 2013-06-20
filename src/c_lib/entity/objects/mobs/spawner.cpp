#include "spawner.hpp"

#include <entity/object/object.hpp>
#include <entity/object/helpers.hpp>
#include <entity/constants.hpp>
#include <entity/objects/mobs/constants.hpp>
#include <entity/components/health.hpp>
#include <entity/components/dimension.hpp>
#include <entity/components/spawner/monster_spawner.hpp>

namespace Entities
{

void load_mob_spawner_data()
{
    EntityType type = ENTITY_MONSTER_SPAWNER;

    entity_data->begin_attaching_to(type);

    ADD_COMPONENT(Position);

    auto dims = ADD_COMPONENT(Dimension);
    dims->height = MONSTER_SPAWNER_HEIGHT;

    auto vox = ADD_COMPONENT(VoxelModel);
    vox->vox_dat = &VoxDats::monster_spawner;
    vox->init_hitscan = MONSTER_SPAWNER_INIT_WITH_HITSCAN;
    vox->init_draw = MONSTER_SPAWNER_INIT_WITH_DRAW;

    auto health = ADD_COMPONENT(HitPoints);
    health->health = MONSTER_SPAWNER_MAX_HEALTH;
    health->health_max = MONSTER_SPAWNER_MAX_HEALTH;

    auto spawner = ADD_COMPONENT(MonsterSpawner);
    spawner->radius = MONSTER_SPAWNER_SPAWN_RADIUS;
    spawner->set_max_children(MONSTER_SPAWNER_MAX_CHILDREN);
    spawner->spawn_type = ENTITY_MONSTER_BOX;

    #if DC_SERVER
    auto item_drop = ADD_COMPONENT(ItemDrop);
    GS_ASSERT(item_drop != NULL);
    item_drop->drop.set_max_drop_types(2);
    item_drop->drop.set_max_drop_amounts("synthesizer_coin", 3);
    item_drop->drop.add_drop("synthesizer_coin", 4, 0.5f);
    item_drop->drop.add_drop("synthesizer_coin", 6, 0.35f);
    item_drop->drop.add_drop("synthesizer_coin", 8, 0.15f);

    item_drop->drop.set_max_drop_amounts("small_charge_pack", 4);
    item_drop->drop.add_drop("small_charge_pack", 1, 0.50f);
    item_drop->drop.add_drop("small_charge_pack", 2, 0.25f);
    item_drop->drop.add_drop("small_charge_pack", 3, 0.15f);
    item_drop->drop.add_drop("small_charge_pack", 4, 0.05f);

    auto limiter = ADD_COMPONENT(RateLimit);
    limiter->limit = MOB_BROADCAST_RATE;
    #endif

    #if DC_CLIENT
    auto anim = ADD_COMPONENT(Animation);
    anim->color = MONSTER_SPAWNER_ANIMATION_COLOR;
    anim->count = MONSTER_SPAWNER_ANIMATION_COUNT;
    //anim->count_max = MONSTER_SPAWNER_ANIMATION_COUNT_MAX;
    anim->size = MONSTER_SPAWNER_ANIMATION_SIZE;
    #endif
}

void ready_mob_spawner(Entity* object)
{

    auto vox = GET_COMPONENT_INTERFACE(VoxelModel, object);
    auto physics = GET_COMPONENT_INTERFACE(Physics, object);

    Vec3 position = physics->get_position();
    Vec3 angles = physics->get_angles();

    vox->ready(position, angles.x, angles.y);
    vox->freeze();

    #if DC_SERVER
    object->broadcastCreate();
    #endif
}

void die_mob_spawner(Entity* object)
{
    #if DC_SERVER
    // drop item
    auto item_drop = GET_COMPONENT_INTERFACE(ItemDrop, object);
    GS_ASSERT(item_drop != NULL);
    item_drop->drop_item();

    object->broadcastDeath();

    auto spawner = GET_COMPONENT(MonsterSpawner, object);
    GS_ASSERT(spawner != NULL);
    spawner->notify_children_of_death();
    #endif

    #if DC_CLIENT
    // explosion animation
    auto vox = GET_COMPONENT_INTERFACE(VoxelModel, object);
    if (vox->vox != NULL)
    {
        auto anim = GET_COMPONENT_INTERFACE(Animation, object);
        anim->explode(vox->get_center());
    }
    #endif
}

void tick_mob_spawner(Entity* object)
{
    #if DC_SERVER
    auto physics = GET_COMPONENT_INTERFACE(Physics, object);
    Vec3 position = physics->get_position();
    position.z = stick_to_terrain_surface(position);
    physics->set_position(position);

    auto limiter = GET_COMPONENT_INTERFACE(RateLimit, object);
    if (limiter->allowed()) object->broadcastState();
    #endif
}

void update_mob_spawner(Entity* object)
{
    auto physics = GET_COMPONENT_INTERFACE(Physics, object);
    auto vox = GET_COMPONENT_INTERFACE(VoxelModel, object);

    Vec3 angles = physics->get_angles();
    vox->force_update(physics->get_position(), angles.x, angles.y, physics->get_changed());
    physics->set_changed(false);  // reset changed state
}

} // Entities

