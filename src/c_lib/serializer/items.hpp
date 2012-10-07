#pragma once

#include <item/item.hpp>

namespace serializer
{

void init_items();
void teardown_items();

int save_player_item(class Item::Item* item, LocationNameID location_name_id, class PlayerItemSaveData* data);

void save_player_container(int client_id, int container_id, bool remove_items_after);

}   // serializer
