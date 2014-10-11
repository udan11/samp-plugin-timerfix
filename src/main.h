/**
 * Copyright (c) 2013-2014, Dan
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met: 
 *
 * 1. Redistributions of source code must retain the above copyright notice, this
 *    list of conditions and the following disclaimer. 
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution. 
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR
 * ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#pragma once

#include <cstdio>
#include <cstdlib>
#include <cstring>

#include <map>
#include <vector>

#if ((defined(WIN32)) || (defined(_WIN32)) || (defined(_WIN64)))
	#include <Windows.h>
#else
	#include <time.h>
#endif

#include "sdk/amx/amx.h"
#include "sdk/amx/amx2.h"
#include "sdk/plugincommon.h"

#include "natives.h"

#ifndef MAX_INT
	#define MAX_INT						2147483647
#endif

#define PLUGIN_VERSION					"v1.4"

#define INVALID_PLAYER_ID				0xFFFF

typedef void (*logprintf_t)(char*, ...);

struct timer {
	AMX *amx;
	int id, playerid, funcidx, interval, repeat;
	unsigned long long next;
	char *func, *format;
	std::vector<std::pair<cell*, int> > params_a;
	std::vector<cell> params_c;
	std::vector<char*> params_s;
};

extern std::map<int, struct timer*> timers;
#ifdef WIN32
extern LARGE_INTEGER startTime;
#else
extern struct timespec startTime;
#endif

extern unsigned long long getMsTime();
extern unsigned long long getRelativeMsTime();

extern int createTimer(AMX *amx, cell playerid, cell funcname, cell interval, cell delay, cell repeat, cell format, cell *params);
extern bool isValidTimer(int id);
extern void freeTimer(struct timer *&t);
extern int executeTimer(struct timer *t);
