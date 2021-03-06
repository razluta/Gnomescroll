/* Gnomescroll, Copyright (c) 2013 Symbolic Analytics
 * Licensed under GPLv3 */
#include "packets.hpp"

#include <chat/_interface.hpp>

#if DC_SERVER
# include <chat/server.hpp>
#endif

namespace Chat
{

#if DC_CLIENT
inline void ChatMessage_StoC::handle()
{
    if (sender != CHAT_SENDER_SYSTEM)
    {   // ignore agents we don't know about
        AgentID agent_id = NetClient::get_agent_id_for_client((ClientID)this->sender);
        Agents::Agent *a = Agents::get_agent(agent_id);
        if (a == NULL) return;
    }

    for (int i=0; i<CHAT_MESSAGE_SIZE_MAX; i++)
    {   // convert tabs to a space
        if (this->msg[i] == '\t')
            this->msg[i] = ' ';
    }

    msg[CHAT_MESSAGE_SIZE_MAX] = '\0';
    chat_client->received_message(channel, (ClientID)this->sender, msg);
}

inline void ChatMessage_CtoS::handle(){}
#endif

#if DC_SERVER
inline void ChatMessage_CtoS::handle()
{
    Agents::Agent* a = NetServer::agents[client_id];
    if (a == NULL) return;

    msg[CHAT_MESSAGE_SIZE_MAX] = '\0';
    chat_server->receive_message(channel, client_id, msg);
}

inline void ChatMessage_StoC::handle(){}
#endif

}   // Chat
