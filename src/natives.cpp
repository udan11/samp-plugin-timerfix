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

#include "natives.h"

#include <cstring>

#include <map>

#include "main.h"
#include "time.h"
#include "timers.h"

cell AMX_NATIVE_CALL Natives::GetTickCount(AMX *amx, cell *params)
{
    return (int) (GetMsTime() % MAX_INT);
}

cell AMX_NATIVE_CALL Natives::IsValidTimer(AMX *amx, cell *params)
{
	if (params[0] < 1 * CELL_SIZE)
    {
        return 0;
    }
    int id = params[1];
    if (TimerExists(id))
    {
        return timers[id]->repeat;
    }
    return false;
}

cell AMX_NATIVE_CALL Natives::GetActiveTimers(AMX *amx, cell *params)
{
    return timers.size();
}

cell AMX_NATIVE_CALL Natives::KillTimer(AMX *amx, cell *params)
{
	if (params[0] < 1 * CELL_SIZE)
    {
        return 0;
    }
    int id = params[1];
	if (TimerExists(id))
    {
        // Scheduling for deletion.
        timers[id]->repeat = 0;
    }
    return 1;
}

cell AMX_NATIVE_CALL Natives::KillPlayerTimers(AMX *amx, cell *params)
{
	if (params[0] < 1 * CELL_SIZE)
    {
        return 0;
    }
    int playerid = params[1];
    if (playerid != INVALID_PLAYER_ID)
    {
        for (std::map<int, struct timer*>::iterator it = timers.begin(), next = it; it != timers.end(); it = next)
        {
            ++next;
            struct timer *t = it->second;
            if (t->playerid == playerid)
            {
                t->repeat = 0;
            }
        }
    }
    return 1;
}

cell AMX_NATIVE_CALL Natives::SetTimer(AMX *amx, cell *params)
{
	if (params[0] < 3 * CELL_SIZE)
    {
        return 0;
    }
    return CreateTimer(amx, INVALID_PLAYER_ID, params[1], params[2], params[2], params[3] ? -1 : 1, NULL, NULL);
}

cell AMX_NATIVE_CALL Natives::SetTimerEx(AMX *amx, cell *params)
{
	if (params[0] < 4 * CELL_SIZE)
    {
        return 0;
    }
    return CreateTimer(amx, INVALID_PLAYER_ID, params[1], params[2], params[2], params[3] ? -1 : 1, params[4], &params[5]);
}

cell AMX_NATIVE_CALL Natives::SetTimer_(AMX *amx, cell *params)
{
	if (params[0] < 4 * CELL_SIZE)
    {
        return 0;
    }
    return CreateTimer(amx, INVALID_PLAYER_ID, params[1], params[2], params[3], params[4], NULL, NULL);
}

cell AMX_NATIVE_CALL Natives::SetTimerEx_(AMX *amx, cell *params)
{
	if (params[0] < 5 * CELL_SIZE)
    {
        return 0;
    }
    return CreateTimer(amx, INVALID_PLAYER_ID, params[1], params[2], params[3], params[4], params[5], &params[6]);
}

cell AMX_NATIVE_CALL Natives::SetPlayerTimer(AMX *amx, cell *params)
{
	if (params[0] < 4 * CELL_SIZE)
    {
        return 0;
    }
    return CreateTimer(amx, params[1], params[2], params[3], params[3], params[4] ? -1 : 1, NULL, NULL);
}

cell AMX_NATIVE_CALL Natives::SetPlayerTimerEx(AMX *amx, cell *params)
{
	if (params[0] < 5 * CELL_SIZE)
    {
        return 0;
    }
    return CreateTimer(amx, params[1], params[2], params[3], params[3], params[4] ? -1 : 1, params[5], &params[6]);
}

cell AMX_NATIVE_CALL Natives::SetPlayerTimer_(AMX *amx, cell *params)
{
	if (params[0] < 5 * CELL_SIZE)
    {
        return 0;
    }
    return CreateTimer(amx, params[1], params[2], params[3], params[4], params[5], NULL, NULL);
}

cell AMX_NATIVE_CALL Natives::SetPlayerTimerEx_(AMX *amx, cell *params)
{
	if (params[0] < 6 * CELL_SIZE)
    {
        return 0;
    }
    return CreateTimer(amx, params[1], params[2], params[3], params[4], params[5], params[6], &params[7]);
}

cell AMX_NATIVE_CALL Natives::GetTimerFunctionName(AMX *amx, cell *params)
{
	if (params[0] < 2 * CELL_SIZE)
    {
        return 0;
    }
    int id = params[1];
	if (!TimerExists(id))
    {
        amx_SetCString(amx, params[2], "", 1); // "\0"
        return 0;
    }
    // TODO: Consider using an additional `len` parameter to avoids overflows.
    amx_SetCString(amx, params[2], timers[id]->func, strlen(timers[id]->func));
    return 1;
}

cell AMX_NATIVE_CALL Natives::SetTimerInterval(AMX *amx, cell *params)
{
	if (params[0] < 2 * CELL_SIZE)
    {
        return 0;
    }
    int id = params[1], interval = params[2];
	if (TimerExists(id))
    {
        timers[id]->interval = interval;
        timers[id]->next = GetMsTime() + interval;
    }
    return 1;
}

cell AMX_NATIVE_CALL Natives::GetTimerInterval(AMX *amx, cell *params)
{
	if (params[0] < 1 * CELL_SIZE)
    {
        return 0;
    }
    int id = params[1];
	if (TimerExists(id))
    {
        return timers[id]->interval;
    }
    return 0;
}

cell AMX_NATIVE_CALL Natives::GetTimerIntervalLeft(AMX *amx, cell *params)
{
	if (params[0] < 1 * CELL_SIZE)
    {
        return 0;
    }
    int id = params[1];
	if (TimerExists(id))
    {
        return timers[id]->next - GetMsTime();
    }
    return 0;
}


cell AMX_NATIVE_CALL Natives::SetTimerDelay(AMX *amx, cell *params)
{
	if (params[0] < 2 * CELL_SIZE)
    {
        return 0;
    }
    int id = params[1], delay = params[2];
	if (TimerExists(id))
    {
        timers[id]->next = GetMsTime() + delay;
    }
    return 1;
}

cell AMX_NATIVE_CALL Natives::SetTimerCount(AMX *amx, cell *params)
{
	if (params[0] < 2 * CELL_SIZE)
    {
        return 0;
    }
    int id = params[1], count = params[2];
	if (TimerExists(id))
    {
        timers[id]->repeat = count;
    }
    return 1;
}

cell AMX_NATIVE_CALL Natives::GetTimerCallsLeft(AMX *amx, cell *params)
{
	if (params[0] < 1 * CELL_SIZE)
    {
        return 0;
    }
    int id = params[1];
	if (TimerExists(id))
    {
        return timers[id]->repeat;
    }
    return 0;
}
