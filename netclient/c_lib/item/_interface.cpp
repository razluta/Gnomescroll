#include "_interface.hpp"

#include <common/crash_report/stack_trace.hpp>

#include <item/item_container.hpp>
#include <item/item.hpp>
#include <item/_state.hpp>
#include <item/particle/_interface.hpp>

#if DC_SERVER
#include <item/net/StoC.hpp>
#endif

#if DC_CLIENT
#include <item/client.hpp>
#endif

namespace Item
{

void init()
{
    item_container_list = new ItemContainerList;
    item_list           = new ItemList;

    //set_sprite_ids();

    #if DC_SERVER
    agent_container_list   = (int*)   malloc(AGENT_MAX * sizeof(int));
    agent_toolbelt_list    = (int*)   malloc(AGENT_MAX * sizeof(int));
    agent_nanite_list      = (int*)   malloc(AGENT_MAX * sizeof(int));
    agent_hand_list        = (ItemID*)malloc(AGENT_MAX * sizeof(ItemID));
    opened_containers      = (int*)   malloc(AGENT_MAX * sizeof(int));
    
    for (int i=0; i<AGENT_MAX; i++) agent_container_list  [i] = NULL_CONTAINER;
    for (int i=0; i<AGENT_MAX; i++) agent_toolbelt_list   [i] = NULL_CONTAINER;
    for (int i=0; i<AGENT_MAX; i++) agent_nanite_list     [i] = NULL_CONTAINER;
    for (int i=0; i<AGENT_MAX; i++) agent_hand_list       [i] = NULL_ITEM;
    for (int i=0; i<AGENT_MAX; i++) opened_containers     [i] = NULL_CONTAINER;
    #endif

    init_properties();
}

void teardown()
{
    if (item_container_list != NULL) delete item_container_list;
    if (item_list           != NULL) delete item_list;

    #if DC_CLIENT
    if (player_container_ui   != NULL) delete player_container_ui;
    if (player_toolbelt_ui    != NULL) delete player_toolbelt_ui;
    if (player_nanite_ui      != NULL) delete player_nanite_ui;
    if (player_craft_bench_ui != NULL) delete player_craft_bench_ui;
    #endif

    #if DC_SERVER
    if (agent_container_list   != NULL) free(agent_container_list);
    if (agent_toolbelt_list    != NULL) free(agent_toolbelt_list);
    if (agent_nanite_list      != NULL) free(agent_nanite_list);
    if (agent_hand_list        != NULL) free(agent_hand_list);
    if (opened_containers      != NULL) free(opened_containers);
    #endif

    tear_down_properties();
}


ItemContainerInterface* get_container(int id)
{
    assert(item_container_list != NULL);
    return item_container_list->get(id);
}

void destroy_container(int id)
{
    ItemContainerInterface* container = get_container(id);
    if (container == NULL) return;
    
    #if DC_CLIENT
    // destroy contents
    for (int i=0; i<container->slot_max; i++)
    {
        if (container->slot[i] == NULL_ITEM) continue;
        destroy_item(container->slot[i]);
    }
    #endif

    item_container_list->destroy(id);
}

ItemContainerType get_container_type(int container_id)
{
    ItemContainerInterface* container = get_container(container_id);
    if (container == NULL) return CONTAINER_TYPE_NONE;
    return container->type;
}

Item* get_item(ItemID id)
{
    return item_list->get(id);
}

// alias for get_item
class Item* get_item_object(ItemID id)
{
    return get_item(id);
}

int get_item_type(ItemID id)
{
    Item* item = get_item(id);
    if (item == NULL) return NULL_ITEM_TYPE;
    return item->type;
}

int get_item_group(ItemID id)
{
    Item* item = get_item(id);
    if (item == NULL) return IG_ERROR;
    return item->group;
}

int get_stack_size(ItemID id)
{   // space used in a stack
    if (id == NULL_ITEM) return 1;
    Item* item = get_item(id);
    assert(item != NULL);
    return item->stack_size;
}

int get_stack_space(ItemID id)
{   // space left in a stack
    if (id == NULL_ITEM) return 0;
    Item* item = get_item(id);
    assert(item != NULL);
    int stack_space = get_max_stack_size(item->type) - item->stack_size;
    assert(stack_space >= 0);
    return stack_space;
}

int get_item_durability(ItemID id)
{
    Item* item = get_item(id);
    if (item == NULL) return NULL_DURABILITY;
    return item->durability;
}

void destroy_item(ItemID id)
{
    Item* item = get_item(id);
    if (item == NULL) return;
    int container_id = item->container_id;
    int slot = item->container_slot;
    ItemContainerInterface* container = get_container(container_id);
    if (container != NULL && slot != NULL_SLOT)
    {
        container->remove_item(slot);
        #if DC_SERVER
        Agent_state* a = STATE::agent_list->get(container->owner);
        if (a != NULL) send_container_remove(container->owner, container_id, slot);
        #endif
    }
    item_list->destroy(id);
}

void merge_item_stack(ItemID src, ItemID dest)
{
    assert(src != NULL_ITEM);
    assert(dest != NULL_ITEM);

    Item* src_item = get_item(src);
    assert(src_item != NULL);
    Item* dest_item = get_item(dest);
    assert(dest_item != NULL);

    // add src's stack to dest
    dest_item->stack_size += src_item->stack_size;
    assert(dest_item->stack_size <= get_max_stack_size(dest_item->type));
}

void merge_item_stack(ItemID src, ItemID dest, int amount)
{
    assert(src != NULL_ITEM);
    assert(dest != NULL_ITEM);
    assert(amount > 0);
    
    Item* src_item = get_item(src);
    assert(src_item != NULL);
    Item* dest_item = get_item(dest);
    assert(dest_item != NULL);

    // add src's stack to dest
    dest_item->stack_size += amount;
    assert(dest_item->stack_size <= get_max_stack_size(dest_item->type));
    
    // remove from src
    src_item->stack_size -= amount;
    assert(src_item->stack_size >= 1);
}

}   // Item
 
// Client
#if DC_CLIENT
namespace Item
{

ItemContainerInterface* create_container(ItemContainerType type, int id)
{
    return item_container_list->create(type, id);
}

void update_container_ui_from_state()
{
    if (player_container_ui != NULL) player_container_ui->load_data(player_container->slot);
    if (player_toolbelt_ui  != NULL) player_toolbelt_ui->load_data(player_toolbelt->slot);
    if (player_nanite_ui    != NULL) player_nanite_ui->load_data(player_nanite->slot);
    if (player_craft_bench_ui != NULL) player_craft_bench_ui->load_data(player_craft_bench->slot);
}

void open_inventory()
{
    // copy state to ui
    update_container_ui_from_state();
}

void close_inventory()
{
    // attempt throw
    mouse_left_click_handler(NULL_CONTAINER, NULL_SLOT, false, false);
}

void open_container(int container_id)
{
    assert(container_id != NULL_CONTAINER);

    printf("open container %d\n", container_id);

    opened_container = container_id;

    // send open packet
    opened_container_event_id = record_container_event(container_id);
    open_container_CtoS msg;
    msg.container_id = container_id;
    msg.event_id = opened_container_event_id;
    msg.send();
}

void close_container()
{
    if (opened_container == NULL_CONTAINER) return;

    printf("close container %d\n", opened_container);

    opened_container = NULL_CONTAINER;
    
    // send packet
    close_container_CtoS msg;
    msg.container_id = opened_container;
    msg.send();
}


int get_event_container_id(int event_id)
{
    assert(event_id >= 0 && event_id < CONTAINER_EVENT_MAX);
    return container_event[event_id];
}

ItemContainerUIInterface* get_container_ui(int container_id)
{
    assert(container_id != NULL_CONTAINER);
    if (player_craft_bench_ui != NULL && player_craft_bench_ui->id == container_id) return player_craft_bench_ui;
    if (player_container_ui   != NULL && player_container_ui->id   == container_id) return player_container_ui;
    if (player_toolbelt_ui    != NULL && player_toolbelt_ui->id    == container_id) return player_toolbelt_ui;
    if (player_nanite_ui      != NULL && player_nanite_ui->id      == container_id) return player_nanite_ui;
    return NULL;
}

ItemID get_toolbelt_item(int slot)
{
    assert(player_toolbelt != NULL);
    assert(slot >= 0 && slot < player_toolbelt->xdim);
    return player_toolbelt->get_item(slot);
}

Item* create_item(int item_type, ItemID item_id)
{
    assert(item_type != NULL_ITEM_TYPE);
    return item_list->create_type(item_type, item_id);
}

ItemID* get_container_contents(int container_id)
{
    ItemContainerInterface* container = get_container(container_id);
    if (container == NULL) return NULL;
    return container->slot;
}

int* get_container_ui_types(int container_id)
{
    ItemContainerUIInterface* container = get_container_ui(container_id);
    if (container == NULL) return NULL;
    return container->slot_type;
}

int* get_container_ui_stacks(int container_id)
{
    ItemContainerUIInterface* container = get_container_ui(container_id);
    if (container == NULL) return NULL;
    return container->slot_stack;
}

int* get_container_ui_durabilities(int container_id)
{
    ItemContainerUIInterface* container = get_container_ui(container_id);
    if (container == NULL) return NULL;
    return container->slot_durability;
}

void set_ui_slot_durability(int container_id, int slot, int durability)
{
    if (slot == NULL_SLOT) return;
    ItemContainerUIInterface* container = get_container_ui(container_id);
    if (container == NULL) return;
    int item_type = container->get_slot_type(slot);
    int item_stack = container->get_slot_stack(slot);
    container->insert_item(slot, item_type, item_stack, durability);
}

void set_ui_slot_stack_size(int container_id, int slot, int stack_size)
{
    if (slot == NULL_SLOT) return;
    ItemContainerUIInterface* container = get_container_ui(container_id);
    if (container == NULL) return;
    int item_type = container->get_slot_type(slot);
    int item_durability = container->get_slot_durability(slot);
    container->insert_item(slot, item_type, stack_size, item_durability);
}

}
#endif 

// Server
#if DC_SERVER

namespace Item
{

ItemID split_item_stack(ItemID src, int amount)
{
    assert(src != NULL_ITEM);
    assert(amount >= 1);

    Item* src_item = get_item(src);
    assert(src_item != NULL);
    src_item->stack_size -= amount;
    assert(src_item->stack_size >= 1);

    Item* new_item = create_item(src_item->type);
    new_item->stack_size = amount;
    return new_item->id;
}

ItemID split_item_stack_in_half(ItemID src)
{
    assert(src != NULL_ITEM);

    Item* src_item = get_item(src);
    assert(src_item != NULL);
    int split_amount = src_item->stack_size / 2;
    assert(split_amount >= 1);  // Do not call this function for a stack with only 1 (cannot split)
    src_item->stack_size -= split_amount;

    Item* new_item = create_item(src_item->type);
    new_item->stack_size = split_amount;
    return new_item->id;
}

bool agent_owns_container(int agent_id, int container_id)
{
    ASSERT_VALID_AGENT_ID(agent_id);
    if (container_id == NULL_CONTAINER) return false;
    if (agent_container_list[agent_id] == container_id) return true;
    if (agent_toolbelt_list[agent_id] == container_id) return true;
    if (agent_nanite_list[agent_id] == container_id) return true;
    return false;
}

bool agent_can_access_container(int agent_id, int container_id)
{
    ASSERT_VALID_AGENT_ID(agent_id);
    ItemContainerInterface* container = get_container(container_id);
    if (container == NULL) return false;
    // owned by other player
    if (container->owner != NO_AGENT && container->owner != agent_id) return false;
    return true;
}

ItemID get_agent_toolbelt_item(int agent_id, int slot)
{
    ASSERT_VALID_AGENT_ID(agent_id);
    int toolbelt_id = get_agent_toolbelt(agent_id);
    if (toolbelt_id == NULL_CONTAINER) return NULL_ITEM;
    ItemContainer* toolbelt = (ItemContainer*)get_container(toolbelt_id);
    if (toolbelt == NULL) return NULL_ITEM;
    return toolbelt->get_item(slot);
}

ItemID get_agent_hand(int agent_id)
{
    ASSERT_VALID_AGENT_ID(agent_id);
    assert(agent_hand_list != NULL);
    return agent_hand_list[agent_id];
}
    
int get_agent_container(int agent_id)
{
    ASSERT_VALID_AGENT_ID(agent_id);
    assert(agent_container_list != NULL);
    return agent_container_list[agent_id];
}

int get_agent_toolbelt(int agent_id)
{
    ASSERT_VALID_AGENT_ID(agent_id);
    assert(agent_toolbelt_list != NULL);
    return agent_toolbelt_list[agent_id];
}

ItemContainerInterface* create_container(ItemContainerType type)
{
    return item_container_list->create(type);
}

void assign_container_to_agent(ItemContainerInterface* container, int* container_list, int agent_id, int client_id)
{
    assert(container != NULL);
    assert(container_list[agent_id] == NULL_ITEM);
    container_list[agent_id] = container->id;
    container->assign_owner(agent_id);
    init_container(container);
    send_container_create(client_id, container->id);
    send_container_assign(client_id, container->id);
}

void assign_containers_to_agent(int agent_id, int client_id)
{
    ASSERT_VALID_AGENT_ID(agent_id);
    
    ItemContainer* agent_container = (ItemContainer*)item_container_list->create(AGENT_CONTAINER);
    assign_container_to_agent(agent_container, agent_container_list, agent_id, client_id);
    
    ItemContainer* agent_toolbelt = (ItemContainer*)item_container_list->create(AGENT_TOOLBELT);
    assign_container_to_agent(agent_toolbelt, agent_toolbelt_list, agent_id, client_id);

    Item* laser_rifle = create_item(get_item_type((char*)"laser_rifle"));
    auto_add_item_to_container(client_id, agent_toolbelt->id, laser_rifle->id);    // this will send the item create

    Item* mining_laser = create_item(get_item_type((char*)"mining_laser"));
    auto_add_item_to_container(client_id, agent_toolbelt->id, mining_laser->id);    // this will send the item create

    #if !PRODUCTION
    // put a grenade launcher in the toolbelt to selt
    Item* grenade_launcher = create_item(get_item_type((char*)"grenade_launcher"));
    auto_add_item_to_container(client_id, agent_toolbelt->id, grenade_launcher->id);    // this will send the item create

    // add a few container blocks
    Item* crate;
    crate = create_item(get_item_type((char*)"crate_1"));
    auto_add_item_to_container(client_id, agent_toolbelt->id, crate->id);
    crate = create_item(get_item_type((char*)"crate_2"));
    auto_add_item_to_container(client_id, agent_toolbelt->id, crate->id);
    crate = create_item(get_item_type((char*)"crate_3"));
    auto_add_item_to_container(client_id, agent_toolbelt->id, crate->id);
    crate = create_item(get_item_type((char*)"crate_3"));
    auto_add_item_to_container(client_id, agent_toolbelt->id, crate->id);
    #endif


    #if !PRODUCTION
    // debug items
    Item* location_pointer = create_item(get_item_type((char*)"location_pointer"));
    agent_toolbelt->insert_item(agent_toolbelt->slot_max-1, location_pointer->id);
    send_container_item_create(client_id, location_pointer->id, agent_toolbelt->id, agent_toolbelt->slot_max-1);

    Item* block_placer = create_item(get_item_type((char*)"block_placer"));
    agent_toolbelt->insert_item(agent_toolbelt->slot_max-2, block_placer->id);
    send_container_item_create(client_id, block_placer->id, agent_toolbelt->id, agent_toolbelt->slot_max-2);
    #endif
    
    ItemContainerNanite* agent_nanite = (ItemContainerNanite*)item_container_list->create(AGENT_NANITE);
    assign_container_to_agent(agent_nanite, agent_nanite_list, agent_id, client_id);
}

Item* create_item(int item_type)
{
    assert(item_type != NULL_ITEM_TYPE);
    return item_list->create_type(item_type);
}

Item* create_item(char* item_name)
{
    int item_type = get_item_type(item_name);
    assert(item_type != NULL_ITEM_TYPE);
    return create_item(item_type);
}

void agent_died(int agent_id)
{
    ASSERT_VALID_AGENT_ID(agent_id);
    Agent_state* a = ServerState::agent_list->get(agent_id);
    if (a == NULL) return;
    assert(a->status.dead);

    // remove items from agent inventory
    assert(agent_container_list != NULL);
    ItemContainer* container = (ItemContainer*)get_container(agent_container_list[agent_id]);
    if (container == NULL) return;
    for (int i=0; i<container->slot_max; i++)
    {
        ItemID item_id = container->slot[i];
        if (item_id == NULL_ITEM) continue;
        container->remove_item(i);
        send_container_remove(a->client_id, container->id, i);
        ItemParticle::throw_agent_item(agent_id, item_id);
    }

    // close container
    assert(opened_containers != NULL);
    if (opened_containers[agent_id] != NULL_CONTAINER)
    {
        send_container_close(agent_id, opened_containers[agent_id]);
        agent_close_container(agent_id, opened_containers[agent_id]);
    }
}

void agent_quit(int agent_id)
{
    ASSERT_VALID_AGENT_ID(agent_id);
    // destroy containers
    agent_died(agent_id);
    destroy_container(agent_container_list[agent_id]);
    destroy_container(agent_toolbelt_list[agent_id]);
    destroy_container(agent_nanite_list[agent_id]);
}

void digest_nanite_food()
{
    for (int i=0; i<AGENT_MAX; i++)
    {
        if (agent_nanite_list[i] == NULL_CONTAINER) continue;
        ItemContainerNanite* nanite = (ItemContainerNanite*)get_container(agent_nanite_list[i]);
        if (nanite == NULL) continue;
        nanite->digest();
    }
}

void purchase_item_from_nanite(int agent_id, int slot)
{
    ASSERT_VALID_AGENT_ID(agent_id);

    assert(agent_nanite_list != NULL);
    assert(agent_hand_list != NULL);

    // if hand is not empty there will be item leaks
    if (agent_hand_list[agent_id] != NULL_ITEM) return;

    // get container
    if (agent_nanite_list[agent_id] == NULL_CONTAINER) return;
    ItemContainerNanite* nanite = (ItemContainerNanite*)get_container(agent_nanite_list[agent_id]);
    if (nanite == NULL) return;

    // transform plain slot to a shopping slot (as used by dat)
    slot = nanite->get_shopping_slot(slot);
    if (slot == NULL_SLOT) return;
    
    // get the store item
    int xslot = slot % nanite->xdim;
    int yslot = slot / nanite->xdim;
    int item_type, cost;
    get_nanite_store_item(nanite->level, xslot, yslot, &item_type, &cost);
    assert(cost >= 0);
    if (item_type == NULL_ITEM_TYPE) return;
    
    // get the coins
    ItemID coins = nanite->get_coins();
    int coin_stack = 0; // coin stack will return 1 for NULL_ITEM, but we want to treat that as 0
    if (coins != NULL_ITEM) coin_stack = get_stack_size(coins);
    if (coin_stack < cost) return;

    // get agent, for sending state to
    Agent_state* a = ServerState::agent_list->get(nanite->owner);

    // create shopped item
    Item* purchase = create_item(item_type);
    if (purchase == NULL) return;

    if (a != NULL) send_item_create(a->client_id, purchase->id);
    // add to hand
    agent_hand_list[agent_id] = purchase->id;
    if (a != NULL) send_hand_insert(a->client_id, purchase->id);

    // update coins
    if (cost)
    {
        if (coin_stack == cost)
        {   // delete coins
            nanite->remove_item(nanite->slot_max-1);
            if (a != NULL) send_container_remove(a->client_id, nanite->id, nanite->slot_max-1);
            destroy_item(coins);
        }
        else
        {   // decrement coin stack
            Item* coin_item = get_item_object(coins);
            assert(coin_item != NULL);
            coin_item->stack_size -= cost;
            if (a != NULL) send_item_state(a->client_id, coins);
        }
    }
}

// returns stack size
int consume_stack_item(ItemID item_id)
{
    assert(item_id != NULL_ITEM);
    int stack_size = get_stack_size(item_id);
    assert(stack_size > 0);
    if (stack_size == 1)
    {
        destroy_item(item_id);
        return 0;
    }
    Item* item = get_item(item_id);
    assert(item != NULL);
    item->stack_size -= 1;
    return item->stack_size;
}

void craft_item_from_bench(int agent_id, int container_id, int craft_slot)
{
    ASSERT_VALID_AGENT_ID(agent_id);
    assert(agent_hand_list != NULL);

    Agent_state* agent = ServerState::agent_list->get(agent_id);
    if (agent == NULL) return;

    // agent does not own container, abort
    if (container_id != NULL_CONTAINER && !agent_owns_container(agent->id, container_id)) return;

    CraftingRecipe* recipe = get_selected_craft_recipe(container_id, craft_slot);
    if (recipe == NULL) return;

    // hand is not empty and cannot stack the output
    ItemID hand_item = agent_hand_list[agent_id];
    bool hand_empty = (hand_item == NULL_ITEM);
    bool hand_can_stack_recipe = (get_item_type(hand_item) == recipe->output && get_stack_space(hand_item) >= 1);
    if (!hand_empty && !hand_can_stack_recipe) return;
        
    // place in hand
    if (hand_empty)
    {
        // create new item of type
        Item* item = create_item(recipe->output);
        if (item == NULL) return;
        send_item_create(agent->client_id, item->id);
        agent_hand_list[agent_id] = item->id;
        send_hand_insert(agent->client_id, item->id);
    }
    else
    {
        // update item stack
        Item* item = get_item(hand_item);
        assert(item != NULL);
        item->stack_size += 1;
        send_item_state(agent->client_id, item->id);
    }

    // remove reagents from container
    // deleting items as needed, modifying others
    consume_crafting_reagents(agent_id, container_id, recipe->id);
}

void consume_crafting_reagents(int agent_id, int container_id, int recipe_id)
{
    assert(container_id != NULL_CONTAINER);
    assert(recipe_id != NULL_CRAFTING_RECIPE);

    ASSERT_VALID_AGENT_ID(agent_id);
    assert(agent_hand_list != NULL);

    Agent_state* agent = ServerState::agent_list->get(agent_id);
    if (agent == NULL) return;

    // agent does not own container, abort
    if (container_id != NULL_CONTAINER && !agent_owns_container(agent->id, container_id)) return;

    ItemContainerCraftingBench* bench = (ItemContainerCraftingBench*)get_container(container_id);
    assert(bench != NULL);

    CraftingRecipe* recipe = get_craft_recipe(recipe_id);
    assert(recipe != NULL);

    // assemble sorted bench inputs
    // we need them sorted so we can correctly decrement from each item
    int input_count = 0;
    ItemID inputs[bench->slot_max];
    int input_types[bench->slot_max];

    // clear initial state
    for (int i=0; i<bench->slot_max; inputs[i++] = NULL_ITEM);
    for (int i=0; i<bench->slot_max; input_types[i++] = NULL_ITEM_TYPE);

    // sort
    for (int i=0; i<bench->slot_max; i++)
    {
        ItemID item_id = bench->get_item(i);
        if (item_id == NULL_ITEM) continue;
        int item_type = get_item_type(item_id);
        assert(item_type != NULL_ITEM_TYPE);

        // insert sorted
        if (input_count == 0)
        {   // degenerate case
            inputs[input_count] = item_id;
            input_types[input_count] = item_type;
        }
        else
        {   // find insertion point
            int j=0;
            for (; j<input_count; j++)
            {
                // comparison is on types
                if (input_types[j] <= item_type) continue;
                // shift forward
                for (int k=input_count; k>j; k--) inputs[k] = inputs[k-1];
                for (int k=input_count; k>j; k--) input_types[k] = input_types[k-1];
                // insert
                inputs[j] = item_id;
                input_types[j] = item_type;
                break;
            }
            
            // insert failed, append to end
            if (j == input_count)
            {
                inputs[j] = item_id;
                input_types[j] = item_type;
            }
        }
        input_count++;
    }

    for (int i=0; i<recipe->reagent_num; i++)
    {   // remove reagents from inputs

        // gather recipe data
        int type = recipe->reagent[i];
        int count = recipe->reagent_count[i];
        assert(type != NULL_ITEM_TYPE);
        assert(count > 0);

        ItemID item_id = inputs[i];
        assert(item_id != NULL_ITEM);
        Item* item = get_item(item_id);
        assert(item != NULL);
        assert(item->stack_size >= count);

        // determine whether to decrement or fully remove item
        if (item->stack_size <= count)
        {   // remove this item
            destroy_item(item->id);
        }
        else
        {   // decrement this item
            item->stack_size -= count;
            send_item_state(agent->client_id, item->id);
        }
    }
}

void send_container_contents(int agent_id, int client_id, int container_id)
{
    ItemContainerInterface* container = get_container(container_id);
    if (container == NULL) return;

    if (!agent_can_access_container(agent_id, container_id)) return;
    
    for (int i=0; i<container->slot_max; i++)
    {
        if (container->slot[i] == NULL_ITEM) continue;
        send_item_create(client_id, container->slot[i]);
    }
}

void container_block_destroyed(int container_id, int x, int y, int z)
{
    assert(container_id != NULL_CONTAINER);

    ItemContainerInterface* container = get_container(container_id);
    if (container == NULL) return;

    // close all opened containers
    assert(opened_containers != NULL);
    for (int i=0; i<AGENT_MAX; i++)
        if (opened_containers[i] == container_id)
            opened_containers[i] = NULL_CONTAINER;

    // queue the container delete packet first
    // the handler will destroy the contents -- then the item_particle create will recreate
    broadcast_container_delete(container_id);

    // dump contents
    for (int i=0; i<container->slot_max; i++)
    {
        if (container->slot[i] == NULL_ITEM) continue;
        container->remove_item(i);
        ItemParticle::dump_container_item(container->slot[i], x,y,z);
        // no need to send container removal packet
    }

    // destroy container
    destroy_container(container_id);
}

bool agent_in_container_range(int agent_id, int container_id)
{
    // get agent position
    ASSERT_VALID_AGENT_ID(agent_id);
    Agent_state* a = ServerState::agent_list->get(agent_id);
    if (a == NULL) return false;

    Vec3 agent_position = a->get_center();

    // get container position, if applicable
    ItemContainerInterface* container = get_container(container_id);
    if (!container_type_is_block(container->type)) return false;

    int position[3];
    t_map::get_container_location(container->id, position);
    Vec3 container_position = vec3_init(position[0], position[1], position[2]);
    container_position = vec3_add(container_position, vec3_init(0.5f, 0.5f, 0.5f));
    
    // do radius check
    if (vec3_distance_squared(agent_position, container_position)
        <= AGENT_CONTAINER_REACH*AGENT_CONTAINER_REACH) return true;
    return false;
}

// check that agents are still in range of containers they are accessing
void check_agents_in_container_range()
{
    using ServerState::agent_list;
    for (int i=0; i<agent_list->n_max; i++)
    {
        if (agent_list->a[i] == NULL) continue;
        Agent_state* a = agent_list->a[i];
        if (opened_containers[a->id] == NULL_CONTAINER) continue;
        if (agent_in_container_range(a->id, opened_containers[a->id])) continue;
        agent_close_container(a->id, opened_containers[a->id]);
        send_container_close(a->id, opened_containers[a->id]);
    }
}

}   // Item

#endif 
