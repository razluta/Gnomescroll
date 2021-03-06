/* Gnomescroll, Copyright (c) 2013 Symbolic Analytics
 * Licensed under GPLv3 */
#include "_state.hpp"

/* Trigger event function pointers */

namespace Toolbelt
{

/* Config data store */

ClickAndHoldBehaviour* click_and_hold = NULL;
ChargeBehaviour* charge = NULL;

/* Trigger function pointer tables */

// common btwn client & server
tickItem* ticks = NULL;
triggerItem* triggers = NULL;
triggerItem* beta_triggers = NULL;

#if DC_CLIENT
beginItem* begin_triggers = NULL;
endItem* end_triggers = NULL;

tickLocalItem* local_ticks = NULL;
triggerLocalItem* local_triggers = NULL;
triggerLocalItem* local_beta_triggers = NULL;
beginLocalItem* local_begin_triggers = NULL;
endLocalItem* local_end_triggers = NULL;
#endif

}    // Toolbelt
