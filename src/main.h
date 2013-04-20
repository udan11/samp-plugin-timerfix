/**
 * SA:MP Plugin - Timerfix
 * Copyright (C) 2013 Dan
 *  
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *  
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *  
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#pragma once

#include <cstdio>
#include <cstdlib>

#include <map>
#include <vector>

#if ((defined(WIN32)) || (defined(_WIN32)) || (defined(_WIN64)))
	#include "windows.h"
#else
	#include <time.h>
#endif

#include "SDK/amx/amx.h"
#include "SDK/plugincommon.h"
#include "SDK/redirect.h"
#include "SDK/string.h"

#include "natives.h"

typedef void (*logprintf_t)(char*, ...);

struct timer {
	AMX *amx;
	int id, funcidx, interval, repeat;
	unsigned long long next;
	char *format;
	std::vector<std::pair<cell*, int> > params_a;
	std::vector<cell> params_c;
	std::vector<char*> params_s;
};

extern std::map<int, struct timer*> timers;

extern int create_timer(AMX *amx, cell funcname, cell interval, cell delay, cell repeat, cell format, cell *params);
extern bool is_valid_timer(int id);
extern void free_timer(struct timer *&t);
extern int execute_timer(struct timer *t);