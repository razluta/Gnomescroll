#pragma once

#include <entity/component/component.hpp>

namespace Components
{

class DestinationTargetingComponent: public TargetingComponent
{
    private:
        Vec3 destination;

    public:
        float stop_proximity;
    
        float speed;
        int max_z_diff;

        float destination_choice_x;
        float destination_choice_y;

        bool at_destination;
        int ticks_to_destination;

        void check_target_alive();

        void choose_destination();

        bool check_at_destination();

        void orient_to_target(Vec3 camera_position);
        
        bool move_on_surface();

        void set_destination(Vec3 dest);
        struct Vec3 get_destination() { return this->destination; }
        
        int get_ticks_to_destination(Vec3 position)
        {
            float len = vec3_length(vec3_sub(this->destination, position));
            int ttd = (int)(ceil(len / this->speed));
            return ttd;
        }


    virtual ~DestinationTargetingComponent() {}
    
    DestinationTargetingComponent()
    : TargetingComponent(COMPONENT_DESTINATION_TARGETING),
    stop_proximity(0.5f),
    speed(1.0f), max_z_diff(128),
    destination_choice_x(1.0f), destination_choice_y(1.0f), 
    destination(vec3_init(0,0,0)), at_destination(false),
    ticks_to_destination(1)
    {}
};

} // Components