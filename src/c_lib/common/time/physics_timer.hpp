/* Gnomescroll, Copyright (c) 2013 Symbolic Analytics
 * Licensed under GPLv3 */
#pragma once

void _START_CLOCK();

int _GET_TICK();

int _GET_MS_TIME();
long _GET_MICROSECOND_TIME();

void _SET_LAST_TICK();
long _LAST_TICK();

void gs_millisleep(int milliseconds);
void gs_microsleep(int microseconds);
