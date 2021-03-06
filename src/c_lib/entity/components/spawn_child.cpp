/* Gnomescroll, Copyright (c) 2013 Symbolic Analytics
 * Licensed under GPLv3 */
#include "spawn_child.hpp"

namespace Components
{

void SpawnChildComponent::notify_parent_of_death()
{
    if (this->parent_id == NULL_ENTITY) return;

    Entities::Entity* obj = Entities::get(this->parent_type, this->parent_id);
    if (obj == NULL) return;

    auto spawner = GET_COMPONENT(MonsterSpawner, obj);
    if (spawner == NULL) return;
    spawner->lose_child(this->entity->type, this->entity->id);
}

void SpawnChildComponent::parent_died()
{
    this->parent_id = NULL_ENTITY;
}

}; // Components
