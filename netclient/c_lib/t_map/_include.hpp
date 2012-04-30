#pragma once

#include <t_map/constants.hpp>
#include <t_map/t_map_class.cpp>
#include <t_map/t_map.cpp>
#include <t_map/t_properties.cpp>

#include <t_map/net/t_CtoS.cpp>
#include <t_map/net/t_StoC.cpp>


#ifdef DC_CLIENT
#include <t_map/t_vbo.cpp>
#include <t_map/t_vbo_draw.cpp>
#include <t_map/t_vbo_update.cpp>

#include <t_map/glsl/cache.cpp>
#include <t_map/glsl/shader.cpp>
#include <t_map/glsl/texture.cpp>
#endif

#ifdef DC_SERVER
#include <t_map/server/map_manager_class.cpp>
#include <t_map/server/manager.cpp>
#include <t_map/server/map_chunk_history.cpp>
#endif

#include <t_map/config/config.hpp>