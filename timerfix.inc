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
 
/**
 * <remarks>Supported parameters:
 * 		a, A = arrays (must be followed by an integer - array's szie);
 * 		b, B = boolean; c, C = character; d, D, i, I = integer;
 * 		s, S = string; t, T = timer's ID
 * </remarks>
 */

/**
 * <summary>Same KillTimer is used for all types of timer.</summary>
 */
#define KillTimer_						KillTimer
 
/**
 * <summary>An improved version of SetTimer.</summary>
 * <param name="func">Name of the public function to call.</param>
 * <param name="interval">Interval in milliseconds.</param>
 * <param name="delay">Time after this timer should be called for the first time.</param>
 * <param name="count">How many times it should repeat before it's killed (-1 for unlimited).</param>
 * <returns>The ID of the timer.</returns>
 */
native SetTimer_(func[], interval, delay, count);

/**
 * <summary>An improved version of SetTimerEx.</summary>
 * <param name="func">Name of the public function to call.</param>
 * <param name="interval">Interval in milliseconds.</param>
 * <param name="delay">Time after this timer should be called for the first time.</param>
 * <param name="count">How many times it should repeat before it's killed (-1 for unlimited).</param>
 * <param name="format">Special format indicating the types of values the timer will pass.</param>
 * <returns>The ID of the timer.</returns>
 */
native SetTimerEx_(func[], interval, delay, count, format[], {Float, _}:...);