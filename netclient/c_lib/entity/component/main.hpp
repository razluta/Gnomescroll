#pragma once

#include <c_lib/entity/constants.hpp>
#include <c_lib/entity/component/component.hpp>
#include <c_lib/entity/component/component_list.hpp>
#include <c_lib/entity/components/include.hpp>

namespace Components
{

/*
 * Adding a component:
 *
 * Add type. Create component, either inheriting exisiting interface or creating one
 * If creating new interface, add interface type
 * typedef list from template, declare extern, intern and alloc/free
 * Add subscribe/unsubscribe to switches
 * 
 */

/* ComponentList typedefs */
 
/* Physics */
const int MAX_PHYSICS_COMPONENTS = 4096;
typedef ComponentList<PositionPhysicsComponent, COMPONENT_POSITION, MAX_PHYSICS_COMPONENTS>
    PositionPhysicsComponentList;
typedef ComponentList<PositionMomentumPhysicsComponent, COMPONENT_POSITION_MOMENTUM, MAX_PHYSICS_COMPONENTS>
    PositionMomentumPhysicsComponentList;

/* Stackables */
const int MAX_STACKABLE_COMPONENTS = 4096;
typedef ComponentList<StackableComponent, COMPONENT_STACKABLE, MAX_STACKABLE_COMPONENTS>
    StackableComponentList;


/* ComponentList declarations */

extern PositionPhysicsComponentList* position_physics_component_list;
extern PositionMomentumPhysicsComponentList* position_momentum_physics_component_list;

extern StackableComponentList* stackable_component_list;
 

/* ComponentList handler switches */

Component* get_switch(ComponentType type);
void release_switch(Component* component);

} // Components
