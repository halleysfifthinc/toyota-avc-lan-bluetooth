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

#define MAC_ADDRESS_LENGTH 6
#define MAC_ADDRESS_STR_LENGTH 18
#define PAIR_COUNT 3

uint8_t bluetooth_connected(void);
void reset_pairings(void);
void bluetooth_init(void);
void bluetooth_config(void);
void bluetooth_connectIndex(uint8_t index);
void bluetooth_enableSearch();
void bluetooth_disableSearch();
void bluetooth_disconnectAll();
uint8_t bluetooth_index(void);
uint8_t bluetooth_is_playing(void);
void bluetooth_next_song(void);
void bluetooth_previous_song(void);
void bluetooth_pause_song(void);
void bluetooth_play_song(void);

typedef enum MATCH_STATES {
							NO_MATCH = 0,
							MATCHING_PAIR_LIST = 1,
							END_PAIR = 2,
							MATCHING_CONNECT = 3,
							MATCHING_CONNECT_ASYNC = 4,
							MATCHING_DISCONNECT = 5,
							SYNTAX_ERROR = 6,
							START_PLAY = 7,
							STOP_PLAY = 8
							} MATCH_STATES;

#endif
