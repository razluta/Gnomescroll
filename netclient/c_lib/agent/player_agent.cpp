#pragma once

#include "player_agent.hpp"
#include <c_lib/agent/agent.hpp>

//#ifdef DC_CLIENT

void PlayerAgent_state::set_PlayerAgent_id(int id) {
    agent_id = id;
}

void PlayerAgent_state::handle_state_snapshot(int seq, float theta, float phi, float x,float y,float z, float vx,float vy,float vz) {
    //printf("should never be called");


    class AgentState ss;

    ss.seq = seq;
    ss.theta = theta;
    ss.phi = phi;
    ss.x=x;ss.y=y;ss.z=z;
    ss.vx=vx;ss.vy=vy;ss.vz=vz;

    int index = seq%AGENT_STATE_HISTORY_SIZE;

    state_history[index] = ss;


    if((state_history_index+1)%AGENT_STATE_HISTORY_SIZE == index) state_history_index = index;
    //do tick and update stuff

    //printf("received snaphshot %i\n", seq);
}

//take outgoing control input and do client side prediction
//seq for prediction will always exceed client side one
void PlayerAgent_state::handle_control_state(int _seq, int _cs, float _theta, float _phi) {
    //printf("control state received: agent=%i, seq=%i, cs=%i \n", id, _seq, _cs);
    

    int index = _seq%128;

/*
    cs[index].seq = _seq;
    cs[index].cs = _cs;
    cs[index].theta = _theta;
    cs[index].phi = _phi;
*/
    //printf("cs_seq= %i, _seq= %i \n", cs_seq, _seq);

    //_tick();

    //printf("control state= %i\n", new_control_state);
}

//set actually sends
void PlayerAgent_state::set_control_state(uint16_t cs, float theta, float phi) {
    if(agent_id == -1) return;  //player agent not set

    cs_seq_local = (cs_seq_local+1) % 256;

    Agent_cs_CtoS csp;

    csp.id = agent_id;
    csp.seq = cs_seq_local;
    csp.cs = cs;
    csp.theta = theta;
    csp.phi = phi;

    csp.send();

}

#define INITIAL_AGENT_INTERPOLATION_WEIGHT 1.0f
static inline float _agent_weight(float t) {
    return INITIAL_AGENT_INTERPOLATION_WEIGHT * pow(AGENT_INTERPOLATION_DECAY, t/TICK_DURATION);
}

static inline float _agent_interp(float s0, float s1, float scale) {
    return scale*(s1 - s0) + s0;
}

// assumes constant time between history states, until delta_t is defined on the states
//void PlayerAgent_state::calculate_interpolate() {
void PlayerAgent_state::calculate_interpolate() {
    interpolate.x = 0.0f;
    interpolate.y = 0.0f;
    interpolate.z = 0.0f;
    interpolate.vx = 0.0f;
    interpolate.vy = 0.0f;
    interpolate.vz = 0.0f;

    AgentState* a = &interpolate;
    float divisor = 0.0f;
    float t = 0.0f;
    float weight;
    
    int i, index;
    for (i=0; i<AGENT_STATE_HISTORY_SIZE; i++) {
        index = (AGENT_STATE_HISTORY_SIZE + state_history_index - i) % AGENT_STATE_HISTORY_SIZE;

        weight = _agent_weight(t);
        divisor += weight;

        a->x += state_history[index].x * weight;
        a->y += state_history[index].y * weight;
        a->z += state_history[index].z * weight;
        a->vx += state_history[index].vx * weight;
        a->vy += state_history[index].vy * weight;
        a->vz += state_history[index].vz * weight;

        t += TICK_DURATION;
        //t += state_history[index].dt;
    }

    a->x /= divisor;
    a->y /= divisor;
    a->z /= divisor;
    a->vx /= divisor;
    a->vy /= divisor;
    a->vz /= divisor;
}

