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
 
#include "natives.h"

cell AMX_NATIVE_CALL Natives::KillTimer(AMX *amx, cell *params) {
	if (params[0] < 4) {
		return 0;
	}
	int id = params[1];
	if (is_valid_timer(id)) {
		// Scheduling for deletion.
		timers[id]->repeat = 0;
		/*
		free_timer(timers[id]);
		timers.erase(id);
		*/
	}
	return 1;
}

cell AMX_NATIVE_CALL Natives::SetTimer(AMX *amx, cell *params) {
	if (params[0] < 3 * 4) {
		return 0;
	}
	return create_timer(amx, params[1], params[2], params[2], params[3] ? -1 : 1, NULL, NULL);
}

cell AMX_NATIVE_CALL Natives::SetTimerEx(AMX *amx, cell *params) {
	if (params[0] < 5 * 4) {
		return 0;
	}
	return create_timer(amx, params[1], params[2], params[2], params[3] ? -1 : 1, params[4], &params[5]);
}

cell AMX_NATIVE_CALL Natives::SetTimer_(AMX *amx, cell *params) {
	if (params[0] < 4 * 4) {
		return 0;
	}
	return create_timer(amx, params[1], params[2], params[3], params[4], NULL, NULL);
}

cell AMX_NATIVE_CALL Natives::SetTimerEx_(AMX *amx, cell *params) {
	if (params[0] < 6 * 4) {
		return 0;
	}
	return create_timer(amx, params[1], params[2], params[3], params[4], params[5], &params[6]);
}