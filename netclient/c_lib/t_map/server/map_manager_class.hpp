#pragma once

#include <c_lib/t_map/t_map_class.hpp>
#include <c_lib/t_map/t_map.hpp>

#include <c_lib/t_map/net/t_StoC.hpp>

namespace t_map
{

const int  DEFAULT_MAP_MANAGER_RADIUS = 3;

const int MAP_MANAGER_ALIAS_LIST_SIZE = 512;

//alias constants
const unsigned short NO_ALIAS = 0xffff;

const unsigned short SUBSCRIBED = 0;
const unsigned UNSUBSCRIBED_NO_DATA = 0xffff;

/*
    state:
    non-subscribed, no data
    non-subscribed, version number
    subscribed

    0xffff
    0x0000
*/
class MAP_MANAGER_ELEMENT
{
    public:
    unsigned short version;

    MAP_MANAGER_ELEMENT()
    {
        version = UNSUBSCRIBED_NO_DATA;    
    }
};

class Map_manager
{
    public:
    class Terrain_map* t;

    int client_id;

    bool needs_update;
    int xpos; //player chunk position
    int ypos;
    int radius;

    int xchunk_dim;
    int ychunk_dim;
    
    struct MAP_MANAGER_ELEMENT* version_list;
    
    int subed_chunks;
    unsigned short alias_list[ MAP_MANAGER_ALIAS_LIST_SIZE ];

    Map_manager(int _client_id)
    {
        needs_update = false;
        client_id = _client_id;
        
        radius = DEFAULT_MAP_MANAGER_RADIUS;

        subed_chunks = 0;
        xpos = 0xffff;
        ypos = 0xffff;

        t = get_map();
        xchunk_dim = t->xchunk_dim;
        ychunk_dim = t->ychunk_dim;

        version_list = new MAP_MANAGER_ELEMENT[xchunk_dim*ychunk_dim];
        for(int i=0; i < MAP_MANAGER_ALIAS_LIST_SIZE; i++) alias_list[i] = NO_ALIAS;
    }

    ~Map_manager()
    {
        delete[] version_list;
    }

    void update();
    void set_position(int x, int y);

    private:

    void sub(int x, int y);
    void unsub(int alias);

    void send_alias(int alias)
    {
        
    }

    void send_compressed_chunk(int index)
    {
        

    }

    void send_delta()
    {
        
    }
};

#include <stdlib.h>

//http://gcc.gnu.org/onlinedocs/gcc-3.4.6/gcc/Min-and-Max.html

void Map_manager::update()
{
    if(needs_update == false) return;
    //sub chunks 

    int imin = (xpos - radius) >= 0;
    int jmin = (ypos - radius) >= 0;

    int imax = (xpos+radius) <= xchunk_dim;
    int jmax = (ypos+radius) <= ychunk_dim;

    printf("imin,jmin= %i %i imax,jmax= %i %i \n", imin,jmin, imax,jmax);
    for(int i=imin;i<imax; i++)
    for(int j=jmin;j<jmax; j++)
    {
        //if( version_list[i*xchunk_dim + y] )
        printf("sub %i %i \n", i,j);
    }

}

//this is chunk position!
void Map_manager::set_position(int x, int y)
{
    if(x != xpos || y != ypos) needs_update = true;
    xpos = x;
    ypos = y;      
}

void Map_manager::sub(int x, int y)
{
    int index = y*xchunk_dim + x;

    int alias=0;
    while( alias_list[alias] != NO_ALIAS) alias++;
    //set alias
    alias_list[alias] = index;
    //grab chunk
    map_history->chunk[index].add_subscriber(client_id, alias, version_list[index].version);

    send_alias(alias);
    send_compressed_chunk(index);

    //send alias to client
    /*
        send alias to client
        compress map chunk

    */
}


void Map_manager::unsub(int alias)
{
    int index = alias_list[alias];

    version_list[index].version = map_history->chunk[index].version; //cache version

    map_history->chunk[index].remove_subscriber(client_id);
}


}