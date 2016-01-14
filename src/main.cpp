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

#include "main.h"

#include <map>

#include "sdk/amx/amx.h"
#include "sdk/amx/amx2.h"
#include "sdk/plugincommon.h"

#include "time.h"
#include "timers.h"
#include "natives.h"

extern void *pAMXFunctions;
logprintf_t logprintf;

const AMX_NATIVE_INFO NATIVES[] =
{
    {"KillPlayerTimers", Natives::KillPlayerTimers},
    {"SetTimer_", Natives::SetTimer_},
    {"SetTimerEx_", Natives::SetTimerEx_},
    {"SetPlayerTimer", Natives::SetPlayerTimer},
    {"SetPlayerTimerEx", Natives::SetPlayerTimerEx},
    {"SetPlayerTimer_", Natives::SetPlayerTimer_},
    {"SetPlayerTimerEx_", Natives::SetPlayerTimerEx_},
    {"GetTimerFunctionName", Natives::GetTimerFunctionName},
    {"SetTimerInterval", Natives::SetTimerInterval},
    {"GetTimerInterval", Natives::GetTimerInterval},
    {"GetTimerIntervalLeft", Natives::GetTimerIntervalLeft},
    {"SetTimerDelay", Natives::SetTimerDelay},
    {"SetTimerCount", Natives::SetTimerCount},
    {"GetTimerCallsLeft", Natives::GetTimerCallsLeft},
    {"IsValidTimer", Natives::IsValidTimer},
    {NULL, NULL}
};

PLUGIN_EXPORT unsigned int PLUGIN_CALL Supports()
{
    return SUPPORTS_VERSION | SUPPORTS_AMX_NATIVES | SUPPORTS_PROCESS_TICK;
}

PLUGIN_EXPORT bool PLUGIN_CALL Load(void **ppData)
{
    logprintf = (logprintf_t) ppData[PLUGIN_DATA_LOGPRINTF];
    pAMXFunctions = ppData[PLUGIN_DATA_AMX_EXPORTS];
    InitTime();
    logprintf("  >> TimerFix " PLUGIN_VERSION " successfully loaded.");
    return true;
}

PLUGIN_EXPORT int PLUGIN_CALL AmxLoad(AMX *amx)
{
    amx_Redirect(amx, "SetTimer", (ucell) Natives::SetTimer, NULL);
    amx_Redirect(amx, "SetTimerEx", (ucell) Natives::SetTimerEx, NULL);
    amx_Redirect(amx, "KillTimer", (ucell) Natives::KillTimer, NULL);
    amx_Redirect(amx, "GetTickCount", (ucell) Natives::GetTickCount, NULL);
    return amx_Register(amx, NATIVES, -1);
}

PLUGIN_EXPORT int PLUGIN_CALL AmxUnload(AMX *amx)
{
    for (std::map<int, struct timer*>::iterator it = timers.begin(), next = it; it != timers.end(); it = next)
    {
        ++next;
        struct timer *t = it->second;
        if (t->amx == amx)
        {
            DestroyTimer(t);
            timers.erase(it);
        }
    }
    return AMX_ERR_NONE;
}

PLUGIN_EXPORT void PLUGIN_CALL Unload()
{
    logprintf("[plugin.timerfix] Plugin successfully unloaded!");
}

PLUGIN_EXPORT void PLUGIN_CALL ProcessTick()
{
    unsigned long long now = GetMsTime();
    for (std::map<int, struct timer*>::iterator it = timers.begin(), next = it; it != timers.end(); it = next)
    {
        ++next;
        struct timer *t = it->second;
        if (t->repeat != 0)
        {
            if (t->next < now)
            {
                t->next += t->interval;
                ExecuteTimer(it->second);
                if (t->repeat > 0)
                {
                    --t->repeat;
                }
            }
        }
        else
        {
            DestroyTimer(t);
            timers.erase(it);
        }
    }
}
