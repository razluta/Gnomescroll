/* Gnomescroll, Copyright (c) 2013 Symbolic Analytics
 * Licensed under GPLv3 */
#pragma once

#include <physics/vec3.hpp>
#include <physics/quadrant.hpp>

namespace Verlet
{

const float dt = 1/30.0f;
const float gravity = -9.8f;

inline void velocity_integrate(struct Vec3& p, struct Vec3& v, const struct Vec3& a, float dt);
inline void velocity_integrate(struct Vec3& p, struct Vec3& v, float dt);

class VerletComponent
{
    public:

        struct Vec3 position;
        struct Vec3 velocity;
        float dampening;
        float mass;
        float box_radius;

    /* Base api */

    struct Vec3 get_position() const
    {
        return this->position;
    }

    bool set_position(float x, float y, float z)
    {
        if (position_is_equal(this->position, x,y,z))
            return false;
        this->position = vec3_init(x,y,z);
        this->position = translate_position(this->position);
        return true;
    }

    bool set_position(struct Vec3 p)
    {
        GS_ASSERT(vec3_is_valid(p));
        return this->set_position(p.x, p.y, p.z);
    }

    struct Vec3 get_momentum() const
    {
        return vec3_scalar_mult(this->velocity, this->mass);
    }

    bool set_momentum(float mx, float my, float mz)
    {
        return this->set_momentum(vec3_init(mx,my,mz));
    }

    bool set_momentum(struct Vec3 momentum)
    {
        this->velocity = vec3_scalar_mult(momentum, 1.0f/this->mass);
        return true;
    }

    float get_height()
    {
        return NULL_HEIGHT;
    }

    struct Vec3 get_angles()
    {
        return NULL_ANGLES;
    }

    bool set_angles(float theta, float phi, float rho)
    {
        return false;
    }

    /* Addition specialization */

    void set_mass(float mass)
    {
        struct Vec3 momentum = this->get_momentum();
        this->mass = mass;
        this->set_momentum(momentum);
    }

    void set_state(float x, float y, float z, float mx, float my, float mz)
    {
        this->set_position(x,y,z);
        this->set_momentum(mx,my,mz);
    }

    void set_state(Vec3 position, Vec3 momentum)
    {
        this->set_position(position);
        this->set_momentum(momentum);
    }

    void load_settings_from(const VerletComponent* verlet)
    {
        this->dampening = verlet->dampening;
        this->mass = verlet->mass;
        this->box_radius = verlet->box_radius;
    }

    bool bounce();
    bool bounce_no_gravity();
    bool bounce_box(float gravity);
    bool bounce_box();
    bool bounce_box_no_gravity();
    bool radial(float xy, float yr);
    bool collide_no_gravity();
    bool collide_no_gravity(class RaytraceData* data, float* dist);
        // *data is required; float* dist is distance travelled;
    void move_no_gravity();

    VerletComponent() :
        position(NULL_POSITION), velocity(NULL_MOMENTUM),
        dampening(1.0f), mass(1.0f), box_radius(1.0f)
    {}
};

}   // Verlet

