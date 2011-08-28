#ifndef t_viz
#define t_viz

#include <compat_gl.h>

#include <stdio.h>
#include <stdlib.h>

#include "t_vbo.h"


#include <t_map/t_map.h>
#include <t_map/t_properties.h>

int _draw_vbo_indicator(float x, float y, float z);

int _start_frame();
int _event(int id);
int _end_frame();
int _draw_perf_graph(float x, float y, float z);

#endif
