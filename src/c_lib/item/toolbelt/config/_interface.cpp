/* Gnomescroll, Copyright (c) 2013 Symbolic Analytics
 * Licensed under GPLv3 */
#include "_interface.hpp"

#include <item/toolbelt/common/types.hpp>
#include <item/toolbelt/_state.hpp>
#include <item/toolbelt/config/_state.hpp>
#include <item/toolbelt/config/config.hpp>

namespace Toolbelt
{

void init_config()
{
    GS_ASSERT(click_and_hold == NULL);
    click_and_hold = (ClickAndHoldBehaviour*)malloc(MAX_ITEM_TYPES * sizeof(*click_and_hold));
    for (int i=0; i<int(MAX_ITEM_TYPES); click_and_hold[i++] = CLICK_HOLD_DEFAULT);

    GS_ASSERT(charge == NULL);
    charge = (ChargeBehaviour*)malloc(MAX_ITEM_TYPES * sizeof(*charge));
    for (int i=0; i<int(MAX_ITEM_TYPES); charge[i++] = CHARGE_DEFAULT);

    GS_ASSERT(ticks         == NULL);
    GS_ASSERT(triggers      == NULL);
    GS_ASSERT(beta_triggers == NULL);

    ticks         = (tickItem*)    calloc(MAX_ITEM_TYPES, sizeof(tickItem));
    triggers      = (triggerItem*) calloc(MAX_ITEM_TYPES, sizeof(triggerItem));
    beta_triggers = (triggerItem*) calloc(MAX_ITEM_TYPES, sizeof(triggerItem));

    #if DC_CLIENT
    GS_ASSERT(begin_triggers       == NULL);
    GS_ASSERT(end_triggers         == NULL);
    GS_ASSERT(local_ticks          == NULL);
    GS_ASSERT(local_triggers       == NULL);
    GS_ASSERT(local_beta_triggers  == NULL);
    GS_ASSERT(local_begin_triggers == NULL);
    GS_ASSERT(local_end_triggers   == NULL);

    begin_triggers       = (beginItem*)        calloc(MAX_ITEM_TYPES, sizeof(beginItem));
    end_triggers         = (endItem*)          calloc(MAX_ITEM_TYPES, sizeof(endItem));
    local_ticks          = (tickLocalItem*)    calloc(MAX_ITEM_TYPES, sizeof(tickLocalItem));
    local_triggers       = (triggerLocalItem*) calloc(MAX_ITEM_TYPES, sizeof(triggerLocalItem));
    local_beta_triggers  = (triggerLocalItem*) calloc(MAX_ITEM_TYPES, sizeof(triggerLocalItem));
    local_begin_triggers = (beginLocalItem*)   calloc(MAX_ITEM_TYPES, sizeof(beginLocalItem));
    local_end_triggers   = (endLocalItem*)     calloc(MAX_ITEM_TYPES, sizeof(endLocalItem));
    #endif

    register_callbacks();
    validate_callbacks();
}

void teardown_config()
{
    free(click_and_hold);
    free(charge);
    free(ticks);
    free(triggers);
    free(beta_triggers);
    #if DC_CLIENT
    free(begin_triggers);
    free(end_triggers);
    free(local_ticks);
    free(local_triggers);
    free(local_beta_triggers);
    free(local_begin_triggers);
    free(local_end_triggers);
    #endif
}


/* Getters */

#define VALIDATE_ITEM_TYPE_RET(item_type, retval) \
    if (item_type == NULL_ITEM_TYPE) item_type = fist_item_type; \
    if (!validate_item_type(item_type)) return retval;

#define VALIDATE_ITEM_TYPE(item_type) VALIDATE_ITEM_TYPE_RET(item_type, NULL)

/* Private validators for use by entry points */
static bool validate_item_type(ItemType item_type)
{    // returns true if item type is valid
    GS_ASSERT(item_type != NULL_ITEM_TYPE);
    if (item_type == NULL_ITEM_TYPE) return false;
    GS_ASSERT(item_type >= 0 && item_type < MAX_ITEM_TYPES);
    if (item_type < 0 || item_type >= MAX_ITEM_TYPES) return false;
    return true;
}

static int validate_item(ItemID item_id)
{    // returns item type
    GS_ASSERT(item_id != NULL_ITEM);
    if (item_id == NULL_ITEM) return NULL_ITEM_TYPE;
    ItemType item_type = Item::get_item_type(item_id);
    if (validate_item_type(item_type)) return item_type;
    return NULL_ITEM_TYPE;
}

ClickAndHoldBehaviour get_item_click_and_hold_behaviour(ItemType item_type)
{
    VALIDATE_ITEM_TYPE_RET(item_type, CLICK_HOLD_DEFAULT);
    return click_and_hold[item_type];
}

ChargeBehaviour get_item_charge_behaviour(ItemType item_type)
{
    VALIDATE_ITEM_TYPE_RET(item_type, CHARGE_DEFAULT);
    return charge[item_type];
}

/* Callback getters */

tickItem get_tick_item_fn(ItemType item_type)
{
    VALIDATE_ITEM_TYPE(item_type);
    return ticks[item_type];
}

triggerItem get_trigger_item_fn(ItemType item_type)
{
    VALIDATE_ITEM_TYPE(item_type);
    return triggers[item_type];
}

triggerItem get_trigger_item_beta_fn(ItemType item_type)
{
    VALIDATE_ITEM_TYPE(item_type);
    return beta_triggers[item_type];
}

#if DC_CLIENT
tickLocalItem get_tick_local_item_fn(ItemType item_type)
{
    VALIDATE_ITEM_TYPE(item_type);
    return local_ticks[item_type];
}

triggerLocalItem get_trigger_local_item_fn(ItemType item_type)
{
    VALIDATE_ITEM_TYPE(item_type);
    return local_triggers[item_type];
}

triggerLocalItem get_trigger_local_item_beta_fn(ItemType item_type)
{
    VALIDATE_ITEM_TYPE(item_type);
    return local_beta_triggers[item_type];
}

beginItem get_begin_item_fn(ItemType item_type)
{
    VALIDATE_ITEM_TYPE(item_type);
    return begin_triggers[item_type];
}

endItem get_end_item_fn(ItemType item_type)
{
    VALIDATE_ITEM_TYPE(item_type);
    return end_triggers[item_type];
}

beginLocalItem get_begin_local_item_fn(ItemType item_type)
{
    VALIDATE_ITEM_TYPE(item_type);
    return local_begin_triggers[item_type];
}

endLocalItem get_end_local_item_fn(ItemType item_type)
{
    VALIDATE_ITEM_TYPE(item_type);
    return local_end_triggers[item_type];
}
#endif

#undef VALIDATE_ITEM_TYPE_RET
#undef VALIDATE_ITEM_TYPE


}    // Toolbelt
