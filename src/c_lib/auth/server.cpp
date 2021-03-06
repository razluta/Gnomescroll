/* Gnomescroll, Copyright (c) 2013 Symbolic Analytics
 * Licensed under GPLv3 */
#include "server.hpp"

#if DC_CLIENT
# error Do not include this file in the client
#endif

#define  __STDC_FORMAT_MACROS
#if __GCC__
# include <inttypes.h>
#else
typedef unsigned char uint8_t;
#endif

#include <auth/lib/hmac.h>
#include <auth/packets.hpp>

namespace Auth
{

// Globals
char* secret_key_str = NULL;          // hex str representation of secret key. read from secret_key_path
unsigned char* secret_key = NULL;   // hex str converted to bytes

// Init loaders
void load_secret_key()
{
    size_t size = 0;
    char* buf = read_file_to_buffer(SECRET_KEY_PATH, &size);
    IF_ASSERT(buf == NULL) return;
    char* _secret_key_str = strip_whitespace(buf);
    GS_ASSERT(strlen(_secret_key_str) == SECRET_KEY_SIZE * 2);
    unsigned char* _secret_key = hexstr_to_char(_secret_key_str);
    IF_ASSERT(_secret_key == NULL) return;
    free(buf);

    GS_ASSERT(_secret_key_str != NULL);
    GS_ASSERT(_secret_key != NULL);

    if (_secret_key_str != NULL && _secret_key != NULL)
    {
        free(secret_key_str);
        free(secret_key);
        secret_key_str = _secret_key_str;
        secret_key = _secret_key;

        size_t len = strlen(secret_key_str);
        GS_ASSERT(len == SECRET_KEY_SIZE * 2);
        for (size_t i=0; i<len; i++)
            GS_ASSERT(isxdigit(secret_key_str[i]));

        size_t i = 0;
        char c = '\0';
        while ((c = secret_key[i] != '\0') && i <= SECRET_KEY_SIZE)
            i++;
        IF_ASSERT(i != SECRET_KEY_SIZE)
            printf("Secret key length was %d and not %d\n", int(i), int(SECRET_KEY_SIZE));
    }
}

uint8_t* compute_hash(const unsigned char* secret_key, const char* msg, const size_t msg_len)
{
    uint8_t* digest = (uint8_t*)calloc(32, sizeof(*digest));
    int ret = hmac_sha256(secret_key, 64, msg, msg_len, digest);
    IF_ASSERT(ret != 0) return NULL;
    return digest;
}

void print_digest(uint8_t* digest)
{   // print the digest
    for (int i=0; i<32; i++)
        printf("%02lx", (unsigned long)digest[i]);
}

void sprint_digest(char* dest, uint8_t* digest)
{   // sprint the digest
    for (int i=0; i<32; i++)
        sprintf(&dest[i*2], "%02lx", (unsigned long)digest[i]);
}

void server_init()
{
    if (!Options::auth) return;
    load_secret_key();
}

void server_update()
{   // check for a new secret key on disk
    if (!Options::auth) return;
    static int _tick = 0;
    if ((_tick++) % SECRET_KEY_REFRESH_RATE != 0) return;
    const char new_key_fn[] = SECRET_KEY_PATH SECRET_KEY_PATH_TMP_EXT;
    // check if file exists
    if (!file_exists(new_key_fn)) return;
    // rename the new key to the main key file name
    GS_RENAME(new_key_fn, SECRET_KEY_PATH);
    // load the new key
    load_secret_key();
}

// Teardown
void server_teardown()
{
    free(secret_key_str);
    free(secret_key);
}

static bool verify_token(const char* _token, UserID* user_id, time_t* expiration_time, char** username)
{
    IF_ASSERT(secret_key == NULL) return false;

    char* token = NULL;
    bool ok = parse_auth_token(_token, user_id, expiration_time, &token, username);
    if (!ok) return false;

    const unsigned int payload_len = AUTH_TOKEN_ID_LENGTH + AUTH_TOKEN_TIMESTAMP_LENGTH + strlen(*username);
    char* payload = (char*)malloc((payload_len+1)*sizeof(char));

    #ifdef _WIN32
    // mingw doesn't support %lld (neither does MSVC2003)
    static const char fmt[] = "%09d%I64d%s";
    #else
    static const char fmt[] = "%09d%lld%s";
    #endif
    snprintf(payload, payload_len+1, fmt, *user_id, (long long)*expiration_time, *username);
    payload[payload_len] = '\0';

    uint8_t* _hash = compute_hash(secret_key, payload, payload_len);
    IF_ASSERT(_hash == NULL)
    {
        free(payload);
        return false;
    }
    char* hash = (char*)malloc((AUTH_TOKEN_HASH_LENGTH+1)*sizeof(char));
    sprint_digest(hash, _hash);
    free(_hash);
    free(payload);

    // do a constant-time token comparison here, to eliminate that side-channel attack
    bool match = true;
    for (size_t i=0; i<AUTH_TOKEN_HASH_LENGTH; i++)
        if (token[i] != hash[i])
            match = false;

    bool expired = auth_token_expired(*expiration_time, AUTH_TOKEN_LIFETIME);

    free(token);
    free(hash);

    ok = (match && !expired);
    if (!ok) free(*username);

    return ok;
}

bool verify_token(const char* _token)
{
    UserID user_id = NULL_USER_ID;
    time_t expiration_time = 0;
    char* username = NULL;
    bool ok = verify_token(_token, &user_id, &expiration_time, &username);
    if (ok) free(username);
    return ok;
}

void received_auth_token(ClientID client_id, const char* token)
{
    if (!Options::auth)
    {
        send_auth_token_valid(client_id);
        return;
    }
    UserID user_id = NULL_USER_ID;
    time_t expiration_time = 0;
    char* username = NULL;
    bool ok = verify_token(token, &user_id, &expiration_time, &username);
    if (ok)
    {
        send_auth_token_valid(client_id);
        NetServer::client_authorized(client_id, user_id, expiration_time, username);
        free(username);
    }
    else
    {
        send_auth_token_invalid(client_id);
        NetServer::client_authorization_failed(client_id);
    }
}

void send_auth_token_valid(ClientID client_id)
{
    auth_token_valid_StoC msg;
    msg.sendToClient(client_id);
}

void send_auth_token_invalid(ClientID client_id)
{
    auth_token_invalid_StoC msg;
    msg.sendToClient(client_id);
}

void send_clock_time(ClientID client_id)
{
    clock_time_StoC msg;
    msg.server_time = (uint64_t)utc_now();
    msg.sendToClient(client_id);
}

}   // Auth
