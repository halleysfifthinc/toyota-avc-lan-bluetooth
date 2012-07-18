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

#ifndef __AVC_MESSAGES_H
#define __AVC_MESSAGES_H

#define HU_ADDRESS                  0x190
#define MY_ADDRESS                  0x360       // CD Changer #1
#define BROADCAST_ADDRESS           0x01FF      // All audio devices
#define LAN_BROADCAST_ADDRESS		0x0FFF
#define CONTROL_FLAGS               0xF

typedef enum MSG_ID{ 
	AUDIO_AUX_IN_USE,
    AUDIO_TUNER_IN_USE,
    AUDIO_TAPE_IN_USE,
    AUDIO_CD_IN_USE,
	AUDIO_VOLUME_STATUS,
	TUNER_STATUS,
	CD_STATUS,
	CONSOLE_OFF,
	
	LAN_STATUS,
    LAN_REGISTER,
    LAN_INIT,
    LAN_CHECK,
	
	CD_CH_LAN_CHECK,
	RESP_LAN_CHECK,
	
	SELF_REGISTER,
	
	CONSOLE_DISK_BUTTON_1,
	RESP_DISK_BUTTON_1,
	CONSOLE_DISK_BUTTON_2,
	RESP_DISK_BUTTON_2_0,
	RESP_DISK_BUTTON_2_1,
	CONSOLE_DISK_PLAY,
	RESP_CONSOLE_DISK_PLAY_0,
	RESP_CONSOLE_DISK_PLAY_1,
	
	SELF_PLAYING_STATUS,
	
	CONSOLE_DISK_STOP,
	RESP_CONSOLE_DISK_STOP_0,
	RESP_CONSOLE_DISK_STOP_1,
	
	CONSOLE_SEEK_UP,
	CONSOLE_SEEK_DOWN,
	CONSOLE_SCAN,
	CONSOLE_BTN_1,
	CONSOLE_BTN_2,
	CONSOLE_BTN_3,
	CONSOLE_BTN_4,
	CONSOLE_BTN_5,
	CONSOLE_BTN_6,
	
	MESSAGE_ID_SIZE
}MSG_ID;

typedef struct MSG_IDENTIFIER{
	MSG_ID msg_id;
	uint8_t broadcast;
	uint16_t master;
	uint16_t slave;
	uint8_t control;
	uint8_t data_size;
	uint8_t data[20];
	uint8_t data_ignore[20];
	char description[20]; 
}MSG_IDENTIFIER;


#endif