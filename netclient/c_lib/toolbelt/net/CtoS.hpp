#pragma once

#include <net_lib/net.hpp>

namespace Toolbelt
{

// set active slot
// alpha trigger
// beta trigger
// reload

class toolbelt_set_slot_CtoS: public FixedSizeReliableNetPacketToServer<toolbelt_set_slot_CtoS>
{
    public:
        uint8_t slot;
        
    inline void packet(char* buff, int* buff_n, bool pack)
    {
        pack_u8(&slot, buff, buff_n, pack);
    }
    inline void handle();
};

class toolbelt_alpha_action_CtoS: public FixedSizeReliableNetPacketToServer<toolbelt_alpha_action_CtoS>
{
    public:
    inline void packet(char* buff, int* buff_n, bool pack)
    {
    }
    inline void handle();
};

class toolbelt_beta_action_CtoS: public FixedSizeReliableNetPacketToServer<toolbelt_beta_action_CtoS>
{
    public:
    inline void packet(char* buff, int* buff_n, bool pack)
    {
    }
    inline void handle();
};

class toolbelt_reload_action_CtoS: public FixedSizeReliableNetPacketToServer<toolbelt_reload_action_CtoS>
{
    public:
    inline void packet(char* buff, int* buff_n, bool pack)
    {
    }
    inline void handle();
};


} // Toolbelt