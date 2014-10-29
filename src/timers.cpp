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

#include "timers.h"

#include <cstring>

#include "main.h"
#include "time.h"

std::map<int, struct timer*> timers;
int lastTimerId = 1;

int CreateTimer(AMX *amx, cell playerid, cell funcname, cell interval, cell delay, cell repeat, cell format, cell *params)
{
    struct timer *t = (struct timer*) malloc(sizeof(struct timer));
    if (t == NULL)
    {
        logprintf("[plugin.timerfix] Cannot allocate memory.");
        return 0;
    }
    memset(t, 0, sizeof(struct timer));
    t->amx = amx;
    t->id = lastTimerId++;
    t->playerid = playerid;
    amx_GetCString(amx, funcname, t->func);
    if (amx_FindPublic(amx, t->func, &t->funcidx))
    {
        logprintf("[plugin.timerfix] %s: Function was not found.", t->func);
        DestroyTimer(t);
        return 0;
    }
    if (interval < 0)
    {
        logprintf("[plugin.timerfix] %s: Interval (%d) must be at least 0.", t->func, interval);
        DestroyTimer(t);
        return 0;
    }
    t->interval = interval;
    t->repeat = repeat;
    if (delay < 0)
    {
        logprintf("[plugin.timerfix] %s: Delay (%d) must be at least 0.", t->func, delay);
        DestroyTimer(t);
        return 0;
    }
    t->next = GetMsTime() + delay;
    if (format != NULL)
    {
        amx_GetCString(amx, format, t->format);
        for (int i = 0, len = strlen(t->format), p = 0; i != len; ++i, ++p)
        {
            switch (t->format[i])
            {
            case 'a':
            case 'A':
                cell *ptr_arr, *ptr_len, *arr, len;
                amx_GetAddr(amx, params[p], &ptr_arr);
                amx_GetAddr(amx, params[p + 1], &ptr_len);
                len = sizeof(cell)* (*ptr_len);
                arr = (cell*)malloc(len);
                if (arr != NULL)
                {
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
            case 'p':
            case 'P':
            case 't':
            case 'T':
                --p; // We didn't read any parameter.
                break;
            case 's':
            case 'S':
                char *str;
                amx_GetCString(amx, params[p], str);
                t->params_s.push_back(str);
                break;
            default:
                logprintf("[plugin.timerfix] %s: Format '%c' is not recognized.", t->func, t->format[i]);
                break;
            }
        }
    }
    timers[t->id] = t;
    return t->id;
}

bool TimerExists(int id)
{
    return timers.find(id) != timers.end();
}

void DestroyTimer(struct timer *&t)
{
    free(t->func);
    free(t->format);
    for (int i = 0, size = t->params_a.size(); i != size; ++i)
    {
        free(t->params_a[i].first);
    }
    t->params_a.clear();
    t->params_c.clear();
    for (int i = 0, size = t->params_s.size(); i != size; ++i)
    {
        free(t->params_s[i]);
    }
    t->params_s.clear();
    free(t);
}

int ExecuteTimer(struct timer *t)
{
    cell ret, amx_addr = -1;
    if (t->format != NULL)
    {
        int a_idx = t->params_a.size(), c_idx = t->params_c.size(), s_idx = t->params_s.size();
        for (int i = strlen(t->format) - 1; i != -1; --i)
        {
            cell tmp;
            switch (t->format[i])
            {
            case 'a':
            case 'A':
                --a_idx;
                amx_PushArray(t->amx, &tmp, NULL, t->params_a[a_idx].first, t->params_a[a_idx].second);
                if (amx_addr == -1)
                {
                    amx_addr = tmp;
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
                amx_Push(t->amx, t->params_c[--c_idx]);
                break;
            case 'p':
            case 'P':
                amx_Push(t->amx, t->playerid);
            case 's':
            case 'S':
                amx_PushString(t->amx, &tmp, NULL, t->params_s[--s_idx], NULL, NULL);
                if (amx_addr == -1)
                {
                    amx_addr = tmp;
                }
                break;
            case 't':
            case 'T':
                amx_Push(t->amx, t->id);
                break;
            }
        }
    }
    amx_Exec(t->amx, &ret, t->funcidx);
    if (amx_addr != -1)
    {
        amx_Release(t->amx, amx_addr);
    }
    return (int)ret;
}
