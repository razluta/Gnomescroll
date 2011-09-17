#ifndef agent_agent_draw_h
#define agent_agent_draw_h

#include <math.h>

#include <compat.h>
#include "../compat_gl.h"

//#include <physics/vector.h>

#include "./agent_list.h"


#ifdef DC_CLIENT
void agent_Draw(struct Agent_state* g);

void agent_draw(); //draw all agents

void draw_agent_aiming_direction(float x, float y, float z, float xangle, float yangle);
void draw_agent_bounding_box(float x, float y, float z, float radius, float head_height, float height);
void draw_agent_cube_selection(int x, int y, int z, int r, int g, int b);
void draw_agent_cube_side_selection(int x, int y, int z, int cx, int cy, int cz, int r, int g, int b);
#endif

#endif
