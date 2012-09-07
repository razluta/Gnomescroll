#pragma once

#include <agent/constants.hpp>

namespace Auth
{
    
typedef enum
{
    AUTH_ERROR_NONE,            // no error
    AUTH_ERROR_TOKEN_MISSING,   // token was NULL, did not exist 
    AUTH_ERROR_TOKEN_MALFORMED, // token failed to parse
    AUTH_ERROR_TOKEN_EXPIRED,   // token had expired
} AuthError;

/* Example auth token:
 * 000000001|1346898331|b2386482b355bd4d5c6cddf01853057f142639f59f994e454ddf2057d87666fb
 *
 * | is the delimiter
 * First piece is user id, formatted to 9 digits
 * Second piece is a unix timestamp representing expiration time (UTC) (always 10 digits)
 * Third piece is the auth token    (always 64 chars)
 *
 * + 2 chars for delimiting
 */

// Switch to gnomescroll.com when site is live
#define GNOMESCROLL_URL "http://127.0.0.1:5002"

#define GNOMESCROLL_LOGIN_PATH "/login"
#define GNOMESCROLL_TOKEN_PATH "/token"

const int MAX_TOKEN_RETRIES = 10;

const char AUTH_TOKEN_COOKIE_NAME[] = "gstoken";    // name of the cookie value set by the auth server
const char AUTH_TOKEN_DELIMITER = '|';  // delimiter for components of the token

const time_t AUTH_TOKEN_LIFETIME = 60 * 60; // 1 hour, in seconds 

const unsigned int AUTH_TOKEN_ID_LENGTH = 9;
const unsigned int AUTH_TOKEN_TIMESTAMP_LENGTH = 10;
const unsigned int AUTH_TOKEN_HASH_LENGTH = 64;
const unsigned int AUTH_TOKEN_USERNAME_MIN_LENGTH = PLAYER_NAME_MIN_LENGTH;
const unsigned int AUTH_TOKEN_USERNAME_MAX_LENGTH = PLAYER_NAME_MAX_LENGTH;
const unsigned int AUTH_TOKEN_PIECES = 4;
const unsigned int AUTH_TOKEN_LENGTH =
    AUTH_TOKEN_ID_LENGTH + AUTH_TOKEN_TIMESTAMP_LENGTH +
    AUTH_TOKEN_HASH_LENGTH + AUTH_TOKEN_USERNAME_MAX_LENGTH +
    AUTH_TOKEN_PIECES - 1;

}   // Auth
