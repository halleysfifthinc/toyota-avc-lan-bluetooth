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
