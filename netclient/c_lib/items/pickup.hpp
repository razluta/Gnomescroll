#pragma once

#include <c_lib/state/server_state.hpp>
#include <c_lib/state/client_state.hpp>
#include <c_lib/objects/common/interface/entity.hpp>
#include <c_lib/objects/common/net/packets.hpp>
#include <c_lib/objects/components/sprite/sprite.hpp>

#include <c_lib/items/constants.hpp>


using namespace ItemDrops;

//forward decl
#if DC_CLIENT
namespace ClientState
{
    extern SpriteList* sprite_list;
    extern ColoredMinivoxList* colored_minivox_list;
}
#endif

namespace ItemDrops
{

/* State */

const int DEFAULT_PICKUP_ITEM_TTL = 30 * 12;    // 12 seconds
const float DEFAULT_PICKUP_ITEM_RADIUS = 2.0f;
const float DEFAULT_PICKUP_ITEM_SCALE = 1.0f;
const float DEFAULT_PICKUP_ITEM_DAMP = 0.1f;    // hardly bounce
const float DEFAULT_PICKUP_ITEM_MASS = 1.0f;

/* Behaviour */

inline void diePickup(int id, Object_types type, int picked_up_by, bool broadcast_death)
{
    #if DC_SERVER
    if (picked_up_by < 0)
        return;

    if (broadcast_death)
    {
        object_picked_up_StoC msg;
        msg.id = id;
        msg.type = type;
        msg.agent_id = picked_up_by;
        msg.broadcast();
    }
    #endif
}

template <class Object>
void tickPickup(ObjectState* state, Object* object, float pickup_radius)
{
    #if DC_SERVER
    Agent_state* agent = nearest_agent_in_range(object->get_position(), pickup_radius);
    if (agent != NULL && agent->status.gain_item(state->id, state->type, state->subtype))
    {   // was picked up, die
        object->was_picked_up(agent->id);
        state->ttl = state->ttl_max;
    }
    #endif
}

/* Reusable Component */

class PickupComponent
{
    public:
        float pickup_radius;
        int picked_up_by;
        bool broadcast_death;

        void was_picked_up(const int agent_id);
        
    PickupComponent()
    : pickup_radius(1.0f), picked_up_by(-1)
    {}

    ~PickupComponent()
    {
    }
};

/* Initializers */

// TODO -- deprecate for physics initializer
void initialize_pickup_object(Object_types type, int subtype, ObjectState* state)
{
    switch (subtype)
    {
        case ItemDrops::GRENADE_REFILL:
            state->mass = GRENADE_REFILL_MASS;
            state->ttl_max = GRENADE_REFILL_TTL;
            state->damp = GRENADE_REFILL_DAMP;
            break;

        case ItemDrops::LASER_REFILL:
            state->mass = LASER_REFILL_MASS;
            state->ttl_max = LASER_REFILL_TTL;
            state->damp = LASER_REFILL_DAMP;
            break;

        default: return;
    }
}

void initialize_pickup_properties(Object_types type, int subtype, PickupComponent* obj)
{
    switch (type)
    {
        case OBJ_TYPE_GEMSTONE:
            obj->pickup_radius = GEMSTONE_PICKUP_RADIUS;
            break;
        default: return;
    }
}


void initialize_sprite_properties(Object_types type, int subtype, SpriteProperties* obj)
{
    int sprite_index = get_object_type_sprite_index(type, subtype);
    obj->sprite_index = sprite_index;
    switch (type)
    {
        case OBJ_TYPE_REFILL:
            switch (subtype)
            {
                case GRENADE_REFILL:
                    obj->scale = GRENADE_REFILL_TEXTURE_SCALE;
                    break;
                case LASER_REFILL:
                    obj->scale = LASER_REFILL_TEXTURE_SCALE;
                    break;                
                default: break;
            }
            break;
            
        case OBJ_TYPE_GEMSTONE:
            obj->scale = GEMSTONE_SCALE;
            break;

        case OBJ_TYPE_MEAT:
            obj->scale = MEAT_SCALE;
            break;

        default: break;
    }
}

void initialize_minivox_properties(Object_types type, int subtype, MinivoxProperties* obj)
{
    switch (type)
    {
        case OBJ_TYPE_BLOCK_DROP:
            switch (subtype)
            {
                case ItemDrops::DIRT:
                    obj->color = DIRT_COLOR;
                    obj->size = DIRT_SIZE;
                    break;
                case ItemDrops::STONE:
                    obj->color = STONE_COLOR;
                    obj->size = STONE_SIZE;
                    break;
                default: break;
            }
            break;
            
        default: break;
    }
    obj->dtheta_speed = MINIVOX_ITEM_ROTATION_THETA;
    obj->dphi_speed = MINIVOX_ITEM_ROTATION_PHI;
}

/* Composition */

class PickupObject: public PickupComponent, public ObjectStateLayer
{
    public:
        VerletComponent spatial;
    
    PickupObject(int id)
    : PickupComponent(), ObjectStateLayer(Objects::create_packet_momentum, Objects::state_packet_momentum, Objects::owned_none, Objects::team_none, Objects::health_none, &spatial)
    {   // TODO: constants should be loaded via dat
        this->_state.id = id;
        this->_state.mass = DEFAULT_PICKUP_ITEM_MASS;
        this->_state.damp = DEFAULT_PICKUP_ITEM_DAMP;
        this->_state.ttl_max = DEFAULT_PICKUP_ITEM_TTL;

        this->pickup_radius = DEFAULT_PICKUP_ITEM_RADIUS;

    }

    ~PickupObject()
    {
    }

    void tick()
    {
        ObjectState* state = this->state();
        this->spatial.verlet_bounce(state->damp);
        tickPickup(state, this, this->pickup_radius);
        tickTTL(state);
    }

    void update() {}

    void born(int subtype)
    {
        this->_state.subtype = subtype;
        ObjectState* state = this->state();
        initialize_pickup_object(state->type, subtype, state);
        initialize_pickup_properties(state->type, subtype, this);
        this->broadcastCreate();
    }

    void die()
    {
        ObjectState* state = this->state();
        diePickup(state->id, state->type, this->picked_up_by, this->broadcast_death);
    }
};

class PickupObjectSprite: public PickupObject, public SpriteComponent
{
    public:

    PickupObjectSprite(int id)
    : PickupObject(id)
    {
        #if DC_CLIENT
        this->sprite_properties.obj = this;
        #endif
    }

    void born(int subtype)
    {
        PickupObject::born(subtype);
        #if DC_CLIENT
        initialize_sprite_properties(this->state()->type, subtype, &this->sprite_properties);
        ClientState::sprite_list->register_object(&this->sprite_properties);
        #endif
    }

    ~PickupObjectSprite()
    {
        #if DC_CLIENT
        ClientState::sprite_list->unregister_object(&this->sprite_properties);
        #endif
    }

    void draw()
    {
    }
};

class PickupObjectMinivox: public PickupObject, public MinivoxComponent
{
    public:

    PickupObjectMinivox(int id)
    : PickupObject(id)
    {
        #if DC_CLIENT
        this->set_rotation(0,0);
        this->minivox_properties.obj = this;
        #endif
    }

    ~PickupObjectMinivox()
    {
        #if DC_CLIENT
        ClientState::colored_minivox_list->unregister_object(&this->minivox_properties);
        #endif
    }

    void born(int subtype)
    {
        PickupObject::born(subtype);
        #if DC_CLIENT
        ObjectState* state = this->state();
        initialize_minivox_properties(state->type, state->subtype, &this->minivox_properties);
        ClientState::colored_minivox_list->register_object(&this->minivox_properties);
        #endif
    }

    void tick()
    {
        #if DC_CLIENT
        this->delta_rotation();
        #endif
        PickupObject::tick();
    }

    void draw()
    {
    }
};

} // ItemDrops
