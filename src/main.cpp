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

#include "main.h"

logprintf_t logprintf;
std::map<int, struct timer*> timers;
int lastTimerId = 1;
extern void *pAMXFunctions;

const AMX_NATIVE_INFO NATIVES[] = {
	{"SetTimer", Natives::SetTimer},
	{"SetTimerEx", Natives::SetTimerEx},
	{"SetTimer_", Natives::SetTimer_},
	{"SetTimerEx_", Natives::SetTimerEx_},
	{"KillTimer", Natives::KillTimer},
	{NULL, NULL}
};

#ifdef WIN32
unsigned long long int freq;
unsigned long long int get_ms_time() {
	LARGE_INTEGER t;
	QueryPerformanceCounter(&t);
	return t.QuadPart / freq;
}
#else
unsigned long long int get_ms_time() {
	struct timespec t;
	clock_gettime(CLOCK_MONOTONIC, &t);
	return t.tv_sec * 1000 + t.tv_nsec / 1000000;
}
#endif

int create_timer(AMX *amx, cell funcname, cell interval, cell delay, cell repeat, cell format, cell *params) {
	struct timer *t = (struct timer*) malloc(sizeof(struct timer));
	if (t == NULL) {
		logprintf("[plugin.timerfix] Cannot allocate memory.");
		return 0;
	}
	memset(t, 0, sizeof(struct timer));
	t->amx = amx;
	t->id = lastTimerId++;
	char *func;
	amx_GetString_(amx, funcname, func);
	bool found = !amx_FindPublic(amx, func, &t->funcidx);
	free(func);
	if (!found) {
		logprintf("[plugin.timerfix] Function (%s) was not found.", func);
		free_timer(t);
		return 0;
	}
	if (interval < 1) {
		logprintf("[plugin.timerfix] Interval (%d) must be at least 1.", interval);
		free_timer(t);
		return 0;
	}
	t->interval = interval;
	t->repeat = repeat;
	if (delay < 1) {
		logprintf("[plugin.timerfix] Delay (%d) must be at least 1.", delay);
		free_timer(t);
		return 0;
	}
	t->next = get_ms_time() + delay;
	if (format != NULL) {
		amx_GetString_(amx, format, t->format);
		for (int i = 0, len = strlen(t->format), p = 0; i != len; ++i, ++p) {
			switch (t->format[i]) {
				case 'a':
				case 'A':
					cell *ptr_arr, *ptr_len, *arr, len;
					amx_GetAddr(amx, params[p], &ptr_arr);
					amx_GetAddr(amx, params[p + 1], &ptr_len);
					len = sizeof(cell) * (*ptr_len);
					arr = (cell*) malloc(len);
					if (arr != NULL) {
						memcpy(arr, ptr_arr, len);
						t->params_a.push_back(std::make_pair(arr, *ptr_len));
					}
					break;
				case 'b':
				case 'B':
				case 'c':
				case 'C':
				case 'd':
				case 'D':
				case 'i':
				case 'I':
				case 'f': 
				case 'F':
					cell *ptr;
					amx_GetAddr(amx, params[p], &ptr);
					t->params_c.push_back(*ptr);
					break;
				case 's':
				case 'S':
					char *str;
					amx_GetString_(amx, params[p], str);
					t->params_s.push_back(str);
					break;
				case 't':
				case 'T':
					--p; // We didn't read any parameter.
					break;
				default: 
					logprintf("[plugin.timerfix] Format '%c' is not recognized.", t->format[i]);
					break;
			}
		}
	}
	timers[t->id] = t;
	return t->id;
}

bool is_valid_timer(int id) {
	return timers.find(id) != timers.end();
}

void free_timer(struct timer *&t) {
	free(t->format);
	for (int i = 0, size = t->params_a.size(); i != size; ++i) {
		free(t->params_a[i].first);
	}
	t->params_a.clear();
	t->params_c.clear();
	for (int i = 0, size = t->params_s.size(); i != size; ++i) {
		free(t->params_s[i]);
	}
	t->params_s.clear();
	free(t);
}

int execute_timer(struct timer *t) {
	cell ret, amx_addr = -1;
	if (t->format != NULL) {
		int a_idx = t->params_a.size(), c_idx = t->params_c.size(), s_idx = t->params_s.size();
		for (int i = strlen(t->format) - 1; i != -1; --i) {
			switch (t->format[i]) {
				case 'a':
				case 'A':
					if (amx_addr < NULL) {
						amx_addr = NULL;
					}
					amx_PushArray(t->amx, &amx_addr, NULL, t->params_a[--a_idx].first, t->params_a[a_idx].second);
					break;
				case 'b':
				case 'B':
				case 'c':
				case 'C':
				case 'd':
				case 'D':
				case 'i':
				case 'I':
				case 'f':
				case 'F':
					amx_Push(t->amx, t->params_c[--c_idx]);
					break;
				case 's':
				case 'S':
					if (amx_addr < NULL) {
						amx_addr = NULL;
					}
					amx_PushString(t->amx, &amx_addr, NULL, t->params_s[--s_idx], NULL, NULL);
					break;
				case 't':
				case 'T':
					amx_Push(t->amx, t->id);
					break;
			}
		}
	}
	amx_Exec(t->amx, &ret, t->funcidx);
	if (amx_addr >= NULL) {
		amx_Release(t->amx, amx_addr);
	}
	return (int) ret;
}

PLUGIN_EXPORT unsigned int PLUGIN_CALL Supports() {
	return SUPPORTS_VERSION | SUPPORTS_AMX_NATIVES | SUPPORTS_PROCESS_TICK;
}

PLUGIN_EXPORT bool PLUGIN_CALL Load(void **ppData) {
	pAMXFunctions = ppData[PLUGIN_DATA_AMX_EXPORTS];
	logprintf = (logprintf_t) ppData[PLUGIN_DATA_LOGPRINTF];
#ifdef WIN32
	LARGE_INTEGER t;
	QueryPerformanceFrequency(&t);
	freq = t.QuadPart / 1000;
#endif
	logprintf(" >> Plugin succesfully loaded!");
	return true;
}

PLUGIN_EXPORT int PLUGIN_CALL AmxLoad(AMX *amx) {
	redirect(amx, "SetTimer", (ucell) Natives::SetTimer, NULL);
	redirect(amx, "SetTimerEx", (ucell) Natives::SetTimerEx, NULL);
	redirect(amx, "KillTimer", (ucell) Natives::KillTimer, NULL);
	return amx_Register(amx, NATIVES, -1);
}

PLUGIN_EXPORT int PLUGIN_CALL AmxUnload(AMX *amx) {
	for (std::map<int, struct timer*>::iterator it = timers.begin(), next = it; it != timers.end(); it = next) {
		++next;
		struct timer *t = it->second;
		if (t->amx == amx) {
			free_timer(t);
			timers.erase(it);
		}
	}
	return AMX_ERR_NONE;
}

PLUGIN_EXPORT void PLUGIN_CALL Unload() {
	for (std::map<int, struct timer*>::iterator it = timers.begin(), next = it; it != timers.end(); it = next) {
		++next;
		free_timer(it->second);
		timers.erase(it);
	}
	logprintf("[plugin.timerfix] Plugin succesfully unloaded!");
}

PLUGIN_EXPORT void PLUGIN_CALL ProcessTick() {
	unsigned long long int now = get_ms_time();
	for (std::map<int, struct timer*>::iterator it = timers.begin(), next = it; it != timers.end(); it = next) {
		++next;
		struct timer *t = it->second;
		if (t->repeat != 0) {
			if (t->next < now) {
				execute_timer(it->second);
				t->next += t->interval;
				if (t->repeat > 0) {
					--t->repeat;
				}
			}
		} else {
			free_timer(t);
			timers.erase(it);
		}
	}
}