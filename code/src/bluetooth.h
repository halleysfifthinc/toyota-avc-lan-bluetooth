	/*
	This file is part of toyota-avc-lan-bluetooth.

    toyota-avc-lan-bluetooth is free software: you can redistribute it and/or modify
    it under the terms of the Lesser GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    toyota-avc-lan-bluetooth is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    Lesser GNU General Public License for more details.

    You should have received a copy of the Lesser GNU General Public License
    along with toyota-avc-lan-bluetooth.  If not, see <http://www.gnu.org/licenses/>.
	*/

#ifndef __BLUETOOTH_H
#define __BLUETOOTH_H

#define MAC_ADDRESS_LENGTH 18

void bluetooth_init(void);
void bluetooth_config(void);
void bluetooth_next_song(void);
void bluetooth_previous_song(void);
void bluetooth_pause_song(void);
void bluetooth_play_song(void);

typedef enum MATCH_STATES {
							NO_MATCH,
							MATCHING_CONNECT,
							MATCHING_LIST,
							FETCHING_MAC,
							} MATCH_STATES;

#endif
