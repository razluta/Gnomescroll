#pragma once


#include <c_lib/common/enum_types.hpp>
#include <c_lib/monsters/constants.hpp>
#include <c_lib/objects/components/target_acquisition/component.hpp>
#include <c_lib/objects/components/voxel/component.hpp>
#include <c_lib/objects/common/interface/layers.hpp>
#include <c_lib/objects/common/api/born.hpp>
#include <c_lib/objects/common/api/update.hpp>
#include <c_lib/objects/common/api/tick.hpp>
#include <c_lib/objects/common/net/packets.hpp>
#include <c_lib/agent/agent.hpp>
#include <c_lib/monsters/slime.hpp>

namespace Monsters {

extern VoxDat box_vox_dat;

void box_shot_object(object_shot_object_StoC* msg);

class Box: public VoxelComponent, public TargetAcquisitionComponent, public MonsterInterface
{
    public:

    void tick()
    {
        // must stay on ground -- apply agent collision
        // wander randomly (TODO: network model with destinations)
        // TODO -- aggro component

        // if see agent, stop, lock target (todo -- target lock)

        ObjectState* state = this->state();
        Vec3 direction;
        Agent_state* agent = this->acquire_target(
            state->id, state->type, this->get_team(), this->camera_z(),
            this->get_position(),
            state->accuracy_bias, state->sight_range,
            state->attack_enemies, state->attack_random,
            &direction
        );
        if (agent == NULL) return;

        // face target
        //Vec3 position = this->get_position();
        float theta,phi;
        vec3_to_angles(direction, &theta, &phi);
        //Vec3 angles = this->get_angles(); // z is unused for Box
        this->set_angles(theta, phi, 0);

        this->broadcastState();
    }

    void die()
    {
        #if DC_CLIENT
        //dieAnimation();   // todo
        if (this->voxel_properties.vox == NULL)
        {
            Vec3 position = this->voxel_properties.vox->get_part(BOX_PART_BODY)->get_center();
            Animations::slime_melt(position.x, position.y, position.z); // TODO
        }
        #endif

        #if DC_SERVER
        //boxDropItem(this->get_position());
        this->broadcastDeath();
        #endif
    }

    void born()
    {
        ObjectState* state = this->state();
        this->voxel_properties.vox = bornVox(this->voxel_properties.vox_dat, state->id, state->type);
        bornSetVox(
            this->voxel_properties.vox,
            this->voxel_properties.init_hitscan,
            this->voxel_properties.init_draw
        );
        bornUpdateVox(this->voxel_properties.vox, this->get_position(),
            this->spatial_properties.angles.x, this->spatial_properties.angles.y); 
    }

    void update()
    {
        updateVox(
            this->voxel_properties.vox, this->get_position(),
            this->spatial_properties.angles, this->spatial_properties.changed
        );
        this->spatial_properties.set_changed(false);
    }
    
    void draw() {/*Empty*/}

    explicit Box(int id)
    {
        this->_state.id = id;

        this->_state.coin_rule = COINS_ANYONE;

        this->_state.motion_proximity_radius = BOX_MOTION_PROXIMITY_RADIUS;

        // todo -- firing rate / properties
        this->_state.fire_rate_limit = BOX_FIRE_RATE;
        this->_state.accuracy_bias = BOX_ACCURACY_BIAS;
        this->_state.sight_range = BOX_SIGHT_RANGE;
        this->_state.attack_enemies = BOX_ATTACK_ONLY_ENEMIES;    // TODO change attack_enemies to attack_only_enemies
        this->_state.attack_random = BOX_ATTACK_RANDOM;

        // target acquisition stuff
        this->attacker_properties.id = id;
        this->attacker_properties.type = OBJ_TYPE_MONSTER_BOX;
        this->attacker_properties.agent_protection_duration = AGENT_BOX_PROTECTION_DURATION;
        this->attacker_properties.agent_damage = BOX_AGENT_DAMAGE;
        this->attacker_properties.block_damage = BOX_TERRAIN_DAMAGE;
        this->attacker_properties.voxel_damage_radius = BOX_VOXEL_DAMAGE_RADIUS;
        this->attacker_properties.terrain_modification_action = t_map::TMA_MONSTER_BOX;
        this->target_acquisition_probability = BOX_TARGET_LOCK_CHANGE;


        this->voxel_properties.init_hitscan = true;
        this->voxel_properties.init_draw = true;
        this->voxel_properties.vox_dat = &box_vox_dat;

        this->spatial_properties.camera_height = BOX_CAMERA_HEIGHT;
        this->spatial_properties.height = BOX_HEIGHT;

        this->health_properties.health = BOX_HEALTH;

        // TODO -- make speed a base property
        // momentum should not be used this way (can be overwriiten, is only init etc)
        this->set_momentum(BOX_SPEED, BOX_SPEED, BOX_SPEED);
    }
};

} // Monsters