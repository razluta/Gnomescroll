/* Gnomescroll, Copyright (c) 2013 Symbolic Analytics
 * Licensed under GPLv3 */
#include "constants.hpp"

#include <item/container/config/_state.hpp>
#include <item/container/config/_interface.hpp>

namespace serializer
{

const char* get_player_container_location_name(ItemContainerType container_type)
{
    if (container_type == ItemContainer::name::hand)
        return PLAYER_HAND_LOCATION_NAME;
    else
    if (container_type == ItemContainer::name::toolbelt)
        return PLAYER_TOOLBELT_LOCATION_NAME;
    else
    if (container_type == ItemContainer::name::inventory)
        return PLAYER_INVENTORY_LOCATION_NAME;
    else
    if (container_type == ItemContainer::name::equipment)
        return PLAYER_EQUIPMENT_LOCATION_NAME;
    else
    if (container_type == ItemContainer::name::synthesizer)
        return PLAYER_SYNTHESIZER_LOCATION_NAME;
    else
    if (container_type == ItemContainer::name::energy_tanks)
        return PLAYER_ENERGY_TANKS_LOCATION_NAME;
    else
    if (container_type == ItemContainer::name::premium_cache)
        return PLAYER_PREMIUM_CACHE_LOCATION_NAME;

    GS_ASSERT(false);
    return NULL;
}

void verify_config()
{
    GS_ASSERT_ABORT(DAT_NAME_MAX_LENGTH >= 32);

    GS_ASSERT_ABORT(strcmp(PLAYER_HAND_LOCATION_NAME,          "player:hand")          == 0);
    GS_ASSERT_ABORT(strcmp(PLAYER_TOOLBELT_LOCATION_NAME,      "player:toolbelt")      == 0);
    GS_ASSERT_ABORT(strcmp(PLAYER_INVENTORY_LOCATION_NAME,     "player:inventory")     == 0);
    GS_ASSERT_ABORT(strcmp(PLAYER_EQUIPMENT_LOCATION_NAME,     "player:equipment")     == 0);
    GS_ASSERT_ABORT(strcmp(PLAYER_SYNTHESIZER_LOCATION_NAME,   "player:synthesizer")   == 0);
    GS_ASSERT_ABORT(strcmp(PLAYER_ENERGY_TANKS_LOCATION_NAME,  "player:energy_tanks")  == 0);
    GS_ASSERT_ABORT(strcmp(PLAYER_PREMIUM_CACHE_LOCATION_NAME, "player:premium_cache") == 0);

    GS_ASSERT_ABORT(strcmp(get_player_container_location_name(ItemContainer::name::hand),          "player:hand")          == 0);
    GS_ASSERT_ABORT(strcmp(get_player_container_location_name(ItemContainer::name::toolbelt),      "player:toolbelt")      == 0);
    GS_ASSERT_ABORT(strcmp(get_player_container_location_name(ItemContainer::name::inventory),     "player:inventory")     == 0);
    GS_ASSERT_ABORT(strcmp(get_player_container_location_name(ItemContainer::name::equipment),     "player:equipment")     == 0);
    GS_ASSERT_ABORT(strcmp(get_player_container_location_name(ItemContainer::name::synthesizer),   "player:synthesizer")   == 0);
    GS_ASSERT_ABORT(strcmp(get_player_container_location_name(ItemContainer::name::energy_tanks),  "player:energy_tanks")  == 0);
    GS_ASSERT_ABORT(strcmp(get_player_container_location_name(ItemContainer::name::premium_cache), "player:premium_cache") == 0);

    // check that all the expected containers are loaded
    // this *could* be in item/container/config, but since the only reason this matters
    // is because of the seriousness of the serializer, i'll leave it here
    GS_ASSERT_ABORT(ItemContainer::container_attributes != NULL);

    #if PRODUCTION
    // TODO -- enable premium_cache
    GS_ASSERT_ABORT(N_PLAYER_CONTAINERS == 6);
    #else
    GS_ASSERT_ABORT(N_PLAYER_CONTAINERS == 7);
    #endif

    bool agent_hand_found         = false;
    bool agent_toolbelt_found     = false;
    bool agent_inventory_found    = false;
    bool agent_equipment_found    = false;
    bool agent_synthesizer_found  = false;
    bool agent_energy_tanks_found = false;
    bool premium_cache_found      = false;

    // make sure all container types are loaded

    for (size_t i=0; i<ItemContainer::container_attributes->max; i++)
    {
        ContainerAttribute* attr = &ItemContainer::container_attributes->properties[i];
        if (attr == NULL || !attr->loaded || !attr->attached_to_agent) continue;
        if (attr->type == ItemContainer::name::hand)
            agent_hand_found = true;
        else
        if (attr->type == ItemContainer::name::toolbelt)
            agent_toolbelt_found = true;
        else
        if (attr->type == ItemContainer::name::inventory)
            agent_inventory_found = true;
        else
        if (attr->type == ItemContainer::name::equipment)
            agent_equipment_found = true;
        else
        if (attr->type == ItemContainer::name::synthesizer)
            agent_synthesizer_found = true;
        else
        if (attr->type == ItemContainer::name::energy_tanks)
            agent_energy_tanks_found = true;
        else
        if (attr->type == ItemContainer::name::premium_cache)
            premium_cache_found = true;
    }

    GS_ASSERT_ABORT(agent_hand_found);
    GS_ASSERT_ABORT(agent_toolbelt_found);
    GS_ASSERT_ABORT(agent_inventory_found);
    GS_ASSERT_ABORT(agent_equipment_found);
    GS_ASSERT_ABORT(agent_synthesizer_found);
    GS_ASSERT_ABORT(agent_energy_tanks_found);
    GS_ASSERT_ABORT(premium_cache_found);

    // length of location names
    GS_ASSERT_ABORT(strlen(PLAYER_HAND_LOCATION_NAME)          <= LOCATION_NAME_MAX_LENGTH);
    GS_ASSERT_ABORT(strlen(PLAYER_TOOLBELT_LOCATION_NAME)      <= LOCATION_NAME_MAX_LENGTH);
    GS_ASSERT_ABORT(strlen(PLAYER_INVENTORY_LOCATION_NAME)     <= LOCATION_NAME_MAX_LENGTH);
    GS_ASSERT_ABORT(strlen(PLAYER_EQUIPMENT_LOCATION_NAME)     <= LOCATION_NAME_MAX_LENGTH);
    GS_ASSERT_ABORT(strlen(PLAYER_SYNTHESIZER_LOCATION_NAME)   <= LOCATION_NAME_MAX_LENGTH);
    GS_ASSERT_ABORT(strlen(PLAYER_ENERGY_TANKS_LOCATION_NAME)  <= LOCATION_NAME_MAX_LENGTH);
    GS_ASSERT_ABORT(strlen(PLAYER_PREMIUM_CACHE_LOCATION_NAME) <= LOCATION_NAME_MAX_LENGTH);

    GS_ASSERT_ABORT(is_valid_location_name(PLAYER_INVENTORY_LOCATION_NAME));
    GS_ASSERT_ABORT(is_valid_location_name(PLAYER_ENERGY_TANKS_LOCATION_NAME));
    GS_ASSERT_ABORT(is_valid_location_name(PLAYER_SYNTHESIZER_LOCATION_NAME));
    GS_ASSERT_ABORT(is_valid_location_name(PLAYER_EQUIPMENT_LOCATION_NAME));
    GS_ASSERT_ABORT(is_valid_location_name(PLAYER_TOOLBELT_LOCATION_NAME));
    GS_ASSERT_ABORT(is_valid_location_name(PLAYER_HAND_LOCATION_NAME));
    GS_ASSERT_ABORT(is_valid_location_name(PLAYER_PREMIUM_CACHE_LOCATION_NAME));

    // semicolon ; is reserved for redis key format, which may need to link back to the redis key
    // space is reserved for string padding
    // dash - is reserved for uuid
    // Actually, never ever change the delimiters
    GS_ASSERT_ABORT(strcmp(TAG_DELIMITER, "=") == 0);
    GS_ASSERT_ABORT(strstr(TAG_DELIMITER, ":") == NULL);
    GS_ASSERT_ABORT(strstr(TAG_DELIMITER, " ") == NULL);
    GS_ASSERT_ABORT(strstr(TAG_DELIMITER, "-") == NULL);
    GS_ASSERT_ABORT(strlen(TAG_DELIMITER) == TAG_DELIMITER_LENGTH);
    GS_ASSERT_ABORT(strstr(TAG_DELIMITER, PROPERTY_DELIMITER)        == NULL);
    GS_ASSERT_ABORT(strstr(TAG_DELIMITER, COLOR_COMPONENT_DELIMITER) == NULL);
    GS_ASSERT_ABORT(TAG_DELIMITER_LENGTH == 1);

    GS_ASSERT_ABORT(strcmp(PROPERTY_DELIMITER, ";") == 0);
    GS_ASSERT_ABORT(strstr(PROPERTY_DELIMITER, ":") == NULL);
    GS_ASSERT_ABORT(strstr(PROPERTY_DELIMITER, " ") == NULL);
    GS_ASSERT_ABORT(strstr(PROPERTY_DELIMITER, "-") == NULL);
    GS_ASSERT_ABORT(strlen(PROPERTY_DELIMITER) == PROPERTY_DELIMITER_LENGTH);
    GS_ASSERT_ABORT(strstr(PROPERTY_DELIMITER, TAG_DELIMITER)             == NULL);
    GS_ASSERT_ABORT(strstr(PROPERTY_DELIMITER, COLOR_COMPONENT_DELIMITER) == NULL);
    GS_ASSERT_ABORT(PROPERTY_DELIMITER_LENGTH == 1);

    GS_ASSERT_ABORT(strcmp(COLOR_COMPONENT_DELIMITER, ",") == 0);
    GS_ASSERT_ABORT(strstr(COLOR_COMPONENT_DELIMITER, ":") == NULL);
    GS_ASSERT_ABORT(strstr(COLOR_COMPONENT_DELIMITER, " ") == NULL);
    GS_ASSERT_ABORT(strstr(COLOR_COMPONENT_DELIMITER, "-") == NULL);
    GS_ASSERT_ABORT(strlen(COLOR_COMPONENT_DELIMITER) == COLOR_COMPONENT_DELIMITER_LENGTH);
    GS_ASSERT_ABORT(strstr(COLOR_COMPONENT_DELIMITER, TAG_DELIMITER)      == NULL);
    GS_ASSERT_ABORT(strstr(COLOR_COMPONENT_DELIMITER, PROPERTY_DELIMITER) == NULL);
    GS_ASSERT_ABORT(COLOR_COMPONENT_DELIMITER_LENGTH == 1);

    GS_ASSERT_ABORT(strcmp(MAP_POSITION_COMPONENT_DELIMITER, ",") == 0);
    GS_ASSERT_ABORT(strstr(MAP_POSITION_COMPONENT_DELIMITER, ":") == NULL);
    GS_ASSERT_ABORT(strstr(MAP_POSITION_COMPONENT_DELIMITER, " ") == NULL);
    GS_ASSERT_ABORT(strstr(MAP_POSITION_COMPONENT_DELIMITER, "-") == NULL);
    GS_ASSERT_ABORT(strlen(MAP_POSITION_COMPONENT_DELIMITER) == MAP_POSITION_COMPONENT_DELIMITER_LENGTH);
    GS_ASSERT_ABORT(strstr(MAP_POSITION_COMPONENT_DELIMITER, TAG_DELIMITER)      == NULL);
    GS_ASSERT_ABORT(strstr(MAP_POSITION_COMPONENT_DELIMITER, PROPERTY_DELIMITER) == NULL);
    GS_ASSERT_ABORT(MAP_POSITION_COMPONENT_DELIMITER_LENGTH == 1);

    GS_ASSERT_ABORT(strcmp(ENTITY_SEPARATOR, "+") == 0);
    GS_ASSERT_ABORT(strcmp(CONTAINER_SEPARATOR, "+") == 0);
    GS_ASSERT_ABORT(CONTAINER_SEPARATOR_LENGTH == 1);

    GS_ASSERT_ABORT(TAG_LENGTH == 3);

    GS_ASSERT_ABORT(strlen(UUID_TAG)                 == TAG_LENGTH);
    GS_ASSERT_ABORT(strlen(NAME_TAG)                 == TAG_LENGTH);
    GS_ASSERT_ABORT(strlen(COLOR_TAG)                == TAG_LENGTH);
    GS_ASSERT_ABORT(strlen(USER_ID_TAG)              == TAG_LENGTH);
    GS_ASSERT_ABORT(strlen(VERSION_TAG)              == TAG_LENGTH);
    GS_ASSERT_ABORT(strlen(DURABILITY_TAG)           == TAG_LENGTH);
    GS_ASSERT_ABORT(strlen(STACK_SIZE_TAG)           == TAG_LENGTH);
    GS_ASSERT_ABORT(strlen(MAP_POSITION_TAG)         == TAG_LENGTH);
    GS_ASSERT_ABORT(strlen(CONTAINER_SLOT_TAG)       == TAG_LENGTH);
    GS_ASSERT_ABORT(strlen(CONTAINER_COUNT_TAG)      == TAG_LENGTH);
    GS_ASSERT_ABORT(strlen(CONTAINER_ITEM_COUNT_TAG) == TAG_LENGTH);

    // THIS NUMBER CAN NEVER GO LOWER
    GS_ASSERT_ABORT(ITEM_LINE_LENGTH                >= 71);
    GS_ASSERT_ABORT(PLAYER_LINE_LENGTH              >= 15);
    GS_ASSERT_ABORT(CONTAINER_LINE_LENGTH           >= 60);
    GS_ASSERT_ABORT(CONTAINER_FILE_LINE_LENGTH      >= 18);
    GS_ASSERT_ABORT(PLAYER_CONTAINER_LINE_LENGTH    >= 51);
    GS_ASSERT_ABORT(MAP_PALETTE_LINE_LENGTH         >= 37);
    GS_ASSERT_ABORT(MECH_PALETTE_LINE_LENGTH        >= 37);

    GS_ASSERT_ABORT(ITEM_FIELD_COUNT             > 0);
    GS_ASSERT_ABORT(PLAYER_FIELD_COUNT           > 0);
    GS_ASSERT_ABORT(CONTAINER_FIELD_COUNT        > 0);
    GS_ASSERT_ABORT(CONTAINER_FILE_FIELD_COUNT   > 0);
    GS_ASSERT_ABORT(PLAYER_CONTAINER_FIELD_COUNT > 0);

    // we need to #define ITEM_UUID_LENGTH as a plain integer, so we can use it in the format string
    // but we really want it to be UUID_STRING_LENGTH
    GS_ASSERT_ABORT(ITEM_UUID_LENGTH == UUID_STRING_LENGTH);

    GS_ASSERT_ABORT(sizeof(WORLD_DATA_FOLDER_DELIMITER) > 1);
    GS_ASSERT_ABORT(WORLD_DATA_FOLDER_PARTS == 3);
}

}   // serializer
