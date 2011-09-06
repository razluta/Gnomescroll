#include "client.h"


struct NetPeer NPserver;
struct Socket client_socket;

//struct timeval timeout;
//fd_set read_flags;
//fd_set write_flags;

//sequence number handling

unsigned char buffer[1500]; //1500 is max ethernet MTU

void init_client() {

    //init_sequence_numbers(&NPserver); /// FIX
    init_sequencer(&NPserver);
    int local_port = 6967;
    struct Socket*s = create_socket(local_port);
    client_socket = *s;
    free(s);

    init_sequencer(&NPserver);
}

struct NetPeer* CLIENT_get_NP() {
    return &NPserver;
}

void set_server(int a, int b, int c, int d, unsigned short port) {
    NPserver.ip =  ( a << 24 ) | ( b << 16 ) | ( c << 8 ) | d;
    NPserver.port = port;

    struct NetPeer* p = create_net_peer_by_remote_IP(a,b,c,d,port);
    NPserver = *p;
    free(p);
}

void send_packet(unsigned char* buff, int n){
    if(NPserver.connected == 0) {
        printf("Cannot send packet, disconnected!\n");
        }
    int sent_bytes = sendto( client_socket.socket, (const char*)buff, n,0, (const struct sockaddr*)&NPserver.address, sizeof(struct sockaddr_in) );
    if ( sent_bytes != n) { printf( "failed to send packet: return value = %i of %i\n", sent_bytes, n );return;}
}

void send_packet2(){

    if(NPserver.connected == 0) {
        printf("Cannot send packet, disconnected!\n");
        return;
        }

    unsigned char header[16];
    int n1 = 0;
    int seq = get_next_sequence_number(&NPserver);

    PACK_uint16_t(NPserver.client_id, header, &n1); //client id
    PACK_uint8_t(1, header, &n1);  //channel 1
    PACK_uint16_t(seq, header, &n1); //sequence number

    //ack string
    PACK_uint16_t(0, header, &n1); //max seq
    PACK_uint32_t(0, header, &n1); //sequence number

    unsigned int value = 5;
    PACK_uint32_t(value, header, &n1);

    printf("Sent packet %i\n", seq);

    //Simulated packet lose

/*
    if(seq%5 == 0) {
        printf("Intentially dropped packet: %i \n", seq);
        return;
    }
*/
    int sent_bytes = sendto( client_socket.socket, (const char*)header, n1,0, (const struct sockaddr*)&NPserver.address, sizeof(struct sockaddr_in) );
    if ( sent_bytes != n1) { printf( "failed to send packet: return value = %i of %i\n", sent_bytes, n1 ); return;}

}


void attempt_connection_with_server() {
    unsigned char buff[6];
    int n=0;
    PACK_uint16_t(65535, buff, &n);
    PACK_uint8_t(255, buff, &n);
    n=6; //must be 6 bytes
    sendto( client_socket.socket, (const char*)buff, n,0, (const struct sockaddr*)&NPserver.address, sizeof(struct sockaddr_in) );
    printf("Client id requested\n");
}

int validate_packet(unsigned char* buff, int n, struct sockaddr_in* from) {
    //check CRC
    if(n <= 0) return 1;
    if(NPserver.connected == 0 && n ==6) { //server connection packet
        int n1=0;
        uint16_t client_id;
        uint16_t channel_id;
        UNPACK_uint16_t(&channel_id, buff, &n1);
        UNPACK_uint16_t(&client_id, buff, &n1);
        printf("Received client id= %i\n",client_id);
        NPserver.client_id = client_id;
        NPserver.connected = 1;
        //server.server_address.sin_addr.s_addr = from->sin_addr.s_addr;
        //server.server_address.sin_port = from->sin_port;
        printf("Client id assigned: %i server: %i:%i\n", client_id, htonl(from->sin_addr.s_addr), ntohs( from->sin_port ));
        return 0;
    }


    if(from->sin_addr.s_addr != NPserver.address.sin_addr.s_addr) {
        unsigned int from_address = ntohl( from->sin_addr.s_addr );
        unsigned short from_port = ntohs( from->sin_port );
        printf("rogue %i byte packet from IP= %i:%i  Server IP = %i:%i\n", n, from_address,from_port, ntohl(NPserver.address.sin_addr.s_addr), ntohs(NPserver.address.sin_port));
        return 1;
    }
    return 1;
    /*
        int n1=0;
        unsigned short message_id, client_id;
        UNPACK_uint16_t(&message_id, buff, &n1);
        UNPACK_uint16_t(&client_id, buff, &n1);
        return 1; /validates
        printf("WTF:validate_packet\n");
    */

    return 0; //does not validate
}

void process_incoming_packets() {
    struct sockaddr_in from;
    socklen_t fromLength = sizeof( from );
    int bytes_received;

    #if PLATFORM == PLATFORM_WINDOWS
    typedef int socklen_t;
    #endif

    while(1) {
        bytes_received = recvfrom(client_socket.socket, buffer, 1500, 0, (struct sockaddr*)&from, &fromLength);
        if(bytes_received <= 0) {return;}
        if(validate_packet(buffer, bytes_received, &from)) {
            process_packet(buffer, bytes_received);
        }
    }
}

void process_packet(unsigned char* buff, int n) {
    if(n==6) return;
    NPserver.ttl = NPserver.ttl_max; //increase ttl if packet received
    int n1=0;

    uint8_t channel_id;
    uint16_t client_id;
    uint16_t sequence_number;

    uint16_t max_seq;
    uint32_t acks;

    uint32_t value;

    n1=0;

    UNPACK_uint16_t(&client_id, buff, &n1); //client id
    UNPACK_uint8_t(&channel_id, buff, &n1);  //channel 1
    UNPACK_uint16_t(&sequence_number, buff, &n1); //sequence number
    //ack
    UNPACK_uint16_t(&max_seq, buff, &n1); //max seq
    UNPACK_uint32_t(&acks, buff, &n1); //sequence number

    UNPACK_uint32_t(&value, buff, &n1);
    //printf("value= %i\n", value);

    //printf("received packet: sequence number %i from server\n", sequence_number);
    process_acks(&NPserver, max_seq, acks);
    //printf("---\n");
}

unsigned char* out_buffer[1500];
unsigned int out_buffer_n;

void send_agent_state_packet() {
    return;
}

void process_outgoing_packets() {
    return;
}

int decrement_ttl() {
    NPserver.ttl -= 1;
    if(NPserver.ttl <= 0) {
        NPserver.connected = 0;
        printf("Connection to server timed out!\n");
        return 1;
    }
    return 0;
}
