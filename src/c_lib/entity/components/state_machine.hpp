#pragma once

#include <entity/constants.hpp>
#include <entity/component/component.hpp>

namespace Components
{

class StateMachineComponent: public Component
{
    public:
        EntityState state;
        stateRouter router;

    virtual ~StateMachineComponent() {}

    StateMachineComponent()
    : Component(COMPONENT_StateMachine, COMPONENT_INTERFACE_StateMachine),
    state(STATE_NONE), router(NULL)
    {}
};

} // Components
