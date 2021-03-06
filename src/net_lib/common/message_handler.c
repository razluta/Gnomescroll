/* Gnomescroll, Copyright (c) 2013 Symbolic Analytics
   Licensed under GPLv3 */
#include "message_handler.h"

#include <net_lib/common/type_pack.hpp>
#include <net_lib/global.hpp>

static size_t* h_client_packet_size;
static size_t* h_server_packet_size;

static pt2handler_client* client_handler_array;
static pt2handler_server* server_handler_array;

static char** client_handler_names;
static char** server_handler_names;

//should disconnect client
void default_handler_function(char* buff, int n, size_t* read_bytes)
{
    //printf("ERROR!\nNo handler for message_id= %i\n", message_id);
    printf("ERROR! No message handler assigned for this message id\n");
    *read_bytes = 0;
}

void register_server_message_handler(int message_id, size_t size, pt2handler_server fptr, const char* name)
{
    if (message_id > 0xFF || message_id < 0)
    {
        printf("register_server_message_handler: message ID invalid\n");
        return;
    }

    IF_ASSERT(server_handler_array == NULL) return;
    if (server_handler_array[message_id] != NULL)
        printf("register_server_message_handler: reassigning message_id %i\n", message_id);

    IF_ASSERT(h_server_packet_size == NULL) return;
    IF_ASSERT(server_handler_array == NULL) return;

    h_server_packet_size[message_id] = size;
    //printf("CtoS: id,size %d,%d\n", message_id, size);
    server_handler_array[message_id] = fptr;

    server_handler_names[message_id] = (char*)malloc(strlen(name) + 1);
    strcpy(server_handler_names[message_id], name);
}

void register_client_message_handler(int message_id, size_t size, pt2handler_client fptr, const char* name)
{
    if (message_id > 0xFF || message_id < 0)
    {
        printf("register_client_message_handler: message ID invalid\n");
        return;
    }
    IF_ASSERT(client_handler_array == NULL) return;
    if (client_handler_array[message_id] != NULL)
        printf("register_client_message_handler: Reassigning message_id %i\n", message_id);

    IF_ASSERT(h_client_packet_size == NULL) return;
    IF_ASSERT(client_handler_array == NULL) return;

    h_client_packet_size[message_id] = size;
    //printf("StoC: id,size %d,%d\n", message_id, size);
    client_handler_array[message_id] = fptr;

    client_handler_names[message_id] = (char*)malloc(strlen(name) + 1);
    strcpy(client_handler_names[message_id], name);
}

void init_message_handler()
{
    #if !NET_STATIC_ARRAYS
    client_handler_names = (char**) calloc(256, sizeof(char*));
    server_handler_names = (char**) calloc(256, sizeof(char*));
    h_client_packet_size = (size_t*) calloc(256, sizeof(size_t));
    h_server_packet_size = (size_t*) calloc(256, sizeof(size_t));
    client_handler_array = (pt2handler_client*)calloc(256, sizeof(pt2handler_server));
    server_handler_array = (pt2handler_server*)calloc(256, sizeof(pt2handler_server));
    #endif

    for (int i=0;i<256;i++)
    {
        server_handler_names[i] = NULL;
        client_handler_names[i] = NULL;
        server_handler_array[i] = NULL;
        client_handler_array[i] = NULL;
        h_server_packet_size[i] = 0;
        h_client_packet_size[i] = 0;
    }
}

void teardown_message_handler()
{
    #if !NET_STATIC_ARRAYS
    for (int i=0; i<256; i++)
    {
        if (client_handler_names != NULL)
            free(client_handler_names[i]);
        if (server_handler_names != NULL)
            free(server_handler_names[i]);
    }
    free(client_handler_names);
    free(server_handler_names);
    free(h_client_packet_size);
    free(h_server_packet_size);
    free(client_handler_array);
    free(server_handler_array);
    #endif
}


#if DC_SERVER
int process_packet_messages(char* buff, size_t* n, size_t max_n, ClientID client_id)
#endif
#if DC_CLIENT
int process_packet_messages(char* buff, size_t* n, size_t max_n)
#endif
{
    size_t size = 0;
    uint8_t message_id = 0xFF;

    size_t read_bytes = 0;

    do
    {
        unpack_message_id(&message_id, buff, n);
        #if DC_SERVER
        size = h_server_packet_size[message_id];
        #endif

        #if DC_CLIENT
        size = h_client_packet_size[message_id];
        #endif

        if ((*n)+size-1 > max_n)
        { // > or >= ?
            printf("ERROR! message processor would read past end of packet\n");

            printf("n= %lu, max_n= %lu, message_id= %i message_size= %lu\n",
                   (long unsigned)*n, (long unsigned)max_n,
                   message_id, (long unsigned)size);
            return -3;
        }


        #if DC_CLIENT
        //remove this!
        if (client_handler_array[message_id] == NULL)
        {
            printf("message_handler error: no handler for message_id= %i\n", message_id);
            return -4;
        }
        client_handler_array[message_id](buff, *n, &read_bytes);
        #endif

        #if DC_SERVER
        //remove this check
        if (server_handler_array[message_id] == NULL)
        {
            printf("message_handler error: no handler for message_id= %i\n", message_id);
            return -4;
        }
        server_handler_array[message_id](buff, *n, &read_bytes, client_id);
        #endif

        if (read_bytes+1 != size)
        {
            printf("ERROR!: message_id= %i, bytes expected= %lu, bytes read= %lu\n",
                   message_id, (long unsigned)size, (long unsigned)read_bytes);
            return -2;
        }

        *n += read_bytes; //works for non fixed sized
        //printf("n= %i, size= %i, read_bytes= %i\n", *n, size, read_bytes);

        //process next message
    } while (*n < max_n);

    //finished procesing messages
    if (*n == max_n)
        return 0;

    //error that should never occur
    if (*n > max_n)
    {   //error, read past buff
        printf("network error!!! Error: read past buffer\n");
        return -1;
    }

    return -5; //should not happen
}

int process_client_map_messages(char* buff, size_t* n, size_t max_n)
{
    #if DC_SERVER
    GS_ASSERT(false);
    printf("process_client_map_messages Error: this should never be called on server\n");
    #endif
    //printf("*n= %i, max_n= %i\n", *n, max_n);
    uint8_t message_id = 0xFF;
    size_t read_bytes = 0;

    do
    {
        unpack_message_id(&message_id, buff, n);

        if (client_handler_array[message_id] == NULL)
        {
            printf("message_handler error: no handler for message_id= %i\n", message_id);
            return -1;
        }
        client_handler_array[message_id](buff, *n, &read_bytes);

        *n += read_bytes; //works for non fixed sized

        //printf("n= %i, size= %i, read_bytes= %i\n", *n, size, read_bytes);
        //printf("2 n= %i, max_n= %i\n", *n, max_n);

    } while (*n < max_n);

    IF_ASSERT(*n != max_n)
        return -1;
    else
        return 0;
}

#if DC_SERVER
int process_large_messages(char* buff, size_t* n, size_t max_n, ClientID client_id)
#endif
#if DC_CLIENT
int process_large_messages(char* buff, size_t* n, size_t max_n)
#endif
{
    printf("WARNING: process_large_messages, received message on large message channel\n");
    return 0;
}

const char* get_server_packet_name(uint8_t message_id)
{
    return server_handler_names[message_id];
}

const char* get_client_packet_name(uint8_t message_id)
{
    return client_handler_names[message_id];
}
