#pragma once

#include "include.hpp"

#include <entity/constants.hpp>

#include <entity/objects/include.cpp>
#include <entity/object/object.cpp>
#include <entity/object/config.cpp>
#include <entity/object/object_list.cpp>
#include <entity/object/object_data_list.cpp>
#include <entity/object/filter.cpp>
#include <entity/object/helpers.cpp>
#include <entity/object/main.cpp>
#include <entity/objects.cpp>

#include <entity/components/include.cpp>
#include <entity/component/component.cpp>
#include <entity/component/component_list.cpp>
#include <entity/component/lists.cpp>
#include <entity/component/lists/owner_list.cpp>
#include <entity/component/main.cpp>
#include <entity/components.cpp>

#include <entity/network/packets.cpp>
#include <entity/network/interfaces.cpp>
#include <entity/network/main.cpp>

#if DC_SERVER
#include <entity/component/lists/agent_spawner_list.cpp>
#endif
