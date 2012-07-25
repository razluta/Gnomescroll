#include "config.hpp"

namespace Objects
{

static class ObjectConfig* object_conf = NULL;

class ObjectConfig
{
    public:
    
    ObjectType type;
    
    unsigned int max;
    objectLoad   loader;
    objectCreate create;
    objectReady  ready;
    objectDie    die;
    
    bool loaded;
    
    ObjectConfig() { this->init(); }
    
    void init()
    {
        this->type = OBJECT_NONE;
        this->max = 0;
        this->loader = NULL;
        this->create = NULL;
        this->ready  = NULL;
        this->die    = NULL;
        this->loaded = false;
    }
};

static struct ObjectConfig c;
static bool started = false;

static void commit_object_config()
{
    GS_ASSERT(c.type != OBJECT_NONE);
    
    // no duplicate types
    GS_ASSERT(object_conf[c.type].loaded == false);
        
    c.loaded = true;
    object_conf[c.type] = c;
    c.init();
}

static void set_object(ObjectType type)
{
    if (started) commit_object_config();
    started = true;
    
    c.type = type;
}

static void register_settings()
{
    GS_ASSERT(!started);
    
    // fabs

    set_object(OBJECT_AGENT_SPAWNER);
    c.loader = &load_agent_spawner_data;
    c.create = &create_agent_spawner;
    c.ready = &ready_agent_spawner;
    c.die = &die_agent_spawner;
    c.max = 1024;
    
    set_object(OBJECT_BASE);
    c.loader = &load_base_data;
    c.create = &create_base;
    c.ready = &ready_base;
    c.die = &die_base;
    c.max = 2;
    
    set_object(OBJECT_TURRET);
    c.loader = &load_turret_data;
    c.create = &create_turret;
    c.ready = &ready_turret;
    c.die = &die_turret;
    c.max = 512;
    
    set_object(OBJECT_ENERGY_CORE);
    c.loader = &load_energy_core_data;
    c.create = &create_energy_core;
    c.ready = &ready_energy_core;
    c.die = &die_energy_core;
    c.max = 1024;
    
    // mobs
    
    set_object(OBJECT_MONSTER_SPAWNER);
    c.loader = &load_mob_spawner_data;
    c.create = &create_mob_spawner;
    c.ready = &ready_mob_spawner;
    c.die = &die_mob_spawner;
    c.max = 64;
    
    set_object(OBJECT_MONSTER_BOX);
    c.loader = &load_mob_robot_box_data;
    c.create = &create_mob_robot_box;
    c.ready = &ready_mob_robot_box;
    c.die = &die_mob_robot_box;
    c.max = 1024;
    
    set_object(OBJECT_MONSTER_BOMB);
    c.loader = &load_mob_bomb_data;
    c.create = &create_mob_bomb;
    c.ready = &ready_mob_bomb;
    c.die = &die_mob_bomb;
    c.max = 512;
    
    commit_object_config(); // finalize
}

static void validate_settings()
{
    for (int i=0; i<MAX_OBJECT_TYPES; i++)
    {   // either everything is set or nothing
        GS_ASSERT(
              (object_conf[i].max    != 0
            && object_conf[i].loader != NULL
            && object_conf[i].create != NULL
            && object_conf[i].ready  != NULL
            && object_conf[i].die    != NULL)
            ||
              (object_conf[i].max    == 0
            && object_conf[i].loader == NULL
            && object_conf[i].create == NULL
            && object_conf[i].ready  == NULL
            && object_conf[i].die    == NULL)
        );
    }
}

void init_config()
{
    GS_ASSERT(object_conf == NULL);
    object_conf = new ObjectConfig[MAX_OBJECT_TYPES];
    
    register_settings();
    validate_settings();
}

void teardown_config()
{
    if (object_conf != NULL) delete[] object_conf;
}

unsigned int get_object_max(ObjectType type)
{
    GS_ASSERT(type >= 0 && type < MAX_OBJECT_TYPES);
    if (type < 0 || type >= MAX_OBJECT_TYPES) return NULL;
    return object_conf[type].max;
}

objectLoad get_object_load_method(ObjectType type)
{
    GS_ASSERT(type >= 0 && type < MAX_OBJECT_TYPES);
    if (type < 0 || type >= MAX_OBJECT_TYPES) return NULL;
    return object_conf[type].loader;
}

objectCreate get_object_create_method(ObjectType type)
{
    GS_ASSERT(type >= 0 && type < MAX_OBJECT_TYPES);
    if (type < 0 || type >= MAX_OBJECT_TYPES) return NULL;
    return object_conf[type].create;
}

objectReady get_object_ready_method(ObjectType type)
{
    GS_ASSERT(type >= 0 && type < MAX_OBJECT_TYPES);
    if (type < 0 || type >= MAX_OBJECT_TYPES) return NULL;
    return object_conf[type].ready;
}

objectDie get_object_die_method(ObjectType type)
{
    GS_ASSERT(type >= 0 && type < MAX_OBJECT_TYPES);
    if (type < 0 || type >= MAX_OBJECT_TYPES) return NULL;
    return object_conf[type].die;
}


}   // Objects