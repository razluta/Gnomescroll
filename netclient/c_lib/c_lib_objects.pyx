cdef extern from "./physics/vector.h":
    struct Vector:
        float x
        float y
        float z

cdef extern from "./objects/grenade.h":
    Vector* _get_grenade_position(int gid)
    void grenade_tick()
    void grenade_draw()
    int create_grenade(int type, float x, float y, float z, float vx, float vy, float vz, int ttl, int ttl_max)
    void destroy_grenade(int gid)

cdef extern from "./objects/neutron.h":
    void neutron_tick()
    void neutron_draw()
    void create_neutron(int type, int energy, float x, float y, float z, float vx, float vy, float vz)

cdef extern from "./objects/blood.h":
    void blood_tick()
    void blood_draw()
    void create_blood(int type, float x, float y, float z, float vx, float vy, float vz)

cdef extern from "./objects/shrapnel.h":
    void shrapnel_tick()
    void shrapnel_draw()
    void create_shrapnel(int type, float x, float y, float z, float vx, float vy, float vz)

cdef extern from "./objects/circuit_tree.h":
    void circuit_tree_generate(int type, int seed)
    void circuit_tree_draw()

cdef extern from "./state/wrapper.hpp":
    void draw_csprays()
    
cdef extern from "./agent/agent_draw.hpp" namespace "AgentDraw":
    #void agent_draw() #draw all agents
    void draw_agent_aiming_direction(float x, float y, float z, float xangle, float yangle)
    void draw_agent_bounding_box(float x, float y, float z, float radius, float head_height, float height)
    void draw_agent_cube_selection(int x, int y, int z, int r, int g, int b)
    void draw_agent_cube_side_selection(int x, int y, int z, int cx, int cy, int cz, int r, int g, int b)

def tick():
    grenade_tick()
    neutron_tick()
    blood_tick()
    shrapnel_tick()

def draw():
    grenade_draw()
    neutron_draw()
    blood_draw()
    shrapnel_draw()

    draw_csprays()

def _create_grenade(float x, float y, float z, float vx, float vy, float vz, int ttl, int ttl_max):
    return create_grenade(1, x,y,z, vx,vy,vz, ttl, ttl_max)

def _destroy_grenade(int gid):
    destroy_grenade(gid)

def _create_neutron(int type, int energy, float x, float y, float z, float vx, float vy, float vz):
    create_neutron(type,energy, x,y,z, vx,vy,vz)

def _create_cspray(int type, float x, float y, float z, float vx, float vy, float vz):
    return
    #create_cspray(type, x,y,z, vx,vy,vz)

def _create_blood(int type, float x, float y, float z, float vx, float vy, float vz):
    create_blood(type, x,y,z, vx,vy,vz)

def _create_shrapnel(int type, float x, float y, float z, float vx, float vy, float vz):
    create_shrapnel(type, x,y,z, vx,vy,vz)

def _generate_circuit_tree(int type, int seed):
    circuit_tree_generate(type, seed)

def _draw_circuit_tree():
    circuit_tree_draw()

def get_grenade_position(int gid):
    cdef Vector* pos
    pos = _get_grenade_position(gid)
    p = [pos.x, pos.y, pos.z]
    return p
