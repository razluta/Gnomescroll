#pragma once

#include <item/config/_interface.hpp>
#include <item/common/struct.hpp>

namespace Item
{

void def_smelting_recipe(const char* item_name);
void add_smelting_product(const char* item_name);
void set_smelting_reagent(const char* item_name, int quantity);
void end_smelting_recipe();

}   // Item

namespace Item
{

void load_smelting_dat()
{
    // Ore
    def_smelting_recipe("copper_bar");
    set_smelting_reagent("copper_ore", 1);
    set_smelting_reagent("copper_ore", 1);
    end_smelting_recipe();

    def_smelting_recipe("copper_bar");
    set_smelting_reagent("copper_ore", 2);
    end_smelting_recipe();

    // TODO -- remove this recipe. its just to test multiple output
    def_smelting_recipe("copper_bar");
    add_smelting_product("copper_bar");
    set_smelting_reagent("copper_ore", 1);
    set_smelting_reagent("copper_ore", 1);
    set_smelting_reagent("copper_ore", 1);
    set_smelting_reagent("copper_ore", 1);
    end_smelting_recipe();

    def_smelting_recipe("gallium_bar");
    set_smelting_reagent("gallium_ore", 1);
    set_smelting_reagent("gallium_ore", 1);
    end_smelting_recipe();

    def_smelting_recipe("gallium_bar");
    set_smelting_reagent("gallium_ore", 2);
    end_smelting_recipe();

    def_smelting_recipe("iron_bar");
    set_smelting_reagent("iron_ore", 1);
    set_smelting_reagent("iron_ore", 1);
    end_smelting_recipe();
    
    def_smelting_recipe("iron_bar");
    set_smelting_reagent("iron_ore", 2);
    end_smelting_recipe();
    
    def_smelting_recipe("iridium_bar");
    set_smelting_reagent("iridium_ore", 1);
    set_smelting_reagent("iridium_ore", 1);
    end_smelting_recipe();

    def_smelting_recipe("iridium_bar");
    set_smelting_reagent("iridium_ore", 2);
    end_smelting_recipe();    
}

}   // Item
