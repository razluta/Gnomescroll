#pragma once

namespace Item
{

class Item
{
    public:

    ItemID id;
    int group; //stack, laser, mining_laser
    int type;  // stone_block, dirt_block, mining_laser_beta,
    
    int durability;
    int stack_size;
    //int sprite_index;

    explicit Item(int id)
    :   id((ItemID)id),
        group(NULL_ITEM_GROUP),
        type(NULL_ITEM_TYPE),
        durability(NULL_DURABILITY),
        stack_size(1)
    {}

};

}

#include <common/template/object_list.hpp>

namespace Item
{

const int ITEM_LIST_MAX = 1024;

class ItemList: public Object_list<Item, ITEM_LIST_MAX>
{
    private:
        const char* name() { return "Item"; }
    public:
        ItemList() { print_list((char*)this->name(), this); }

        #if DC_CLIENT
        Item* create()
        {
            printf("must create item with id\n");
            assert(false);
            return NULL;
        }
        #endif

        #if DC_SERVER
        Item* create_type(int item_type)
        {
            Item* item = this->create();
            if (item == NULL) return NULL;
            item->type = item_type;
            return item;
        }
        #endif
        
        Item* create_type(int item_type, int item_id)
        {
            Item* item = Object_list<Item, ITEM_LIST_MAX>::create(item_id);
            if (item == NULL) return NULL;
            item->type = item_type;
            return item;
        }
        
        Item* get_or_create_type(int item_type, int item_id)
        {
            Item* item = this->get_or_create(item_id);
            if (item == NULL) return NULL;
            item->type = item_type;
            return item;
        }

        void draw() {}
        void tick() {}
};

}