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

#include <avr/pgmspace.h>
#include "AVC_messages.h"


MSG_IDENTIFIER message_identification_list [] PROGMEM = {

	{ AUDIO_AUX_IN_USE,   	0, HU_ADDRESS, LAN_BROADCAST_ADDRESS, 		CONTROL_FLAGS, 4, {0x11, 0x01, 0x45, 0x01}, {}, "AUX in use" },
    { AUDIO_TUNER_IN_USE, 	0, HU_ADDRESS, LAN_BROADCAST_ADDRESS, 		CONTROL_FLAGS, 4, {0x11, 0x01, 0x45, 0x60}, {}, "Tuner in use" },
    { AUDIO_TAPE_IN_USE,  	0, HU_ADDRESS, LAN_BROADCAST_ADDRESS, 		CONTROL_FLAGS, 4, {0x11, 0x01, 0x45, 0x61}, {}, "Tape in use" },
    { AUDIO_CD_IN_USE,   	0, HU_ADDRESS, LAN_BROADCAST_ADDRESS, 		CONTROL_FLAGS, 4, {0x11, 0x01, 0x45, 0x62}, {}, "CD in use" },	
	{ AUDIO_VOLUME_STATUS,  0, HU_ADDRESS, BROADCAST_ADDRESS	, 		CONTROL_FLAGS, 16, {0x74, 0x31, 0xF1, 0x80, '?', '?', '?', '?', '?', '?', '?', '?', '?', '?', 0x03, 0x00}, {0, 0, 0, 0, '?', '?', '?', '?', '?', '?', '?', '?', '?', '?', 0, 0}, "Volume/Mixer Info"}, 
	{ TUNER_STATUS,         0, HU_ADDRESS, BROADCAST_ADDRESS	, 		CONTROL_FLAGS, 13, {0x60, 0x31, 0xF1, 0x01, '?', '?', '?', '?', '?', '?', 0x00, 0x00, 0x00}, {0, 0, 0, 0, '?', '?', '?', '?', '?', '?', 0, 0, 0}, "Tuner Status"},
	{ CD_STATUS, 			0, HU_ADDRESS, BROADCAST_ADDRESS	, 		CONTROL_FLAGS, 11, {0x62, 0x31, 0xF1, 0x01, 0x10, '?', '?', '?', '?', 0x00, 0x80}, {0, 0, 0, 0, 0, '?', '?', '?', '?', 0, 0}, "CD Playing Status"},
	{ CONSOLE_OFF, 			0, HU_ADDRESS, LAN_BROADCAST_ADDRESS,		CONTROL_FLAGS, 3, {0x11, 0x01, 0x46}, {}, "Console is off" },
	
	{ LAN_STATUS,       	0, HU_ADDRESS, LAN_BROADCAST_ADDRESS, 		CONTROL_FLAGS, 3, {0x00, 0x01, 0x0A}, {}, "LAN Status" },
    { LAN_REGISTER,     	0, HU_ADDRESS, LAN_BROADCAST_ADDRESS, 		CONTROL_FLAGS, 3, {0x11, 0x01, 0x00}, {}, "LAN Register" },
    { LAN_INIT,         	0, HU_ADDRESS, LAN_BROADCAST_ADDRESS, 		CONTROL_FLAGS, 3, {0x11, 0x01, 0x01}, {}, "LAN Restart" },
    { LAN_CHECK,        	0, HU_ADDRESS, LAN_BROADCAST_ADDRESS, 		CONTROL_FLAGS, 4, {0x11, 0x01, 0x20, '?'}, {0, 0, 0, '?'}, "LAN Check" },
	
	{ CD_CH_LAN_CHECK,     	1, HU_ADDRESS, MY_ADDRESS, 					CONTROL_FLAGS, 5, {0x00, 0x11, 0x01, 0x20, '?'}, {0, 0, 0, 0, '?'}, "LAN Check For Me" },
	{ RESP_LAN_CHECK, 1, MY_ADDRESS, HU_ADDRESS,		 				CONTROL_FLAGS, 6, {0x00, 0x01, 0x11, 0x30, '?', 0x0}, {0, 0, 0, 0, '?', 0}, "LAN Check Response"},
	
	{ SELF_REGISTER, 		1, MY_ADDRESS, HU_ADDRESS, 					CONTROL_FLAGS, 5, {0x00, 0x01, 0x11, 0x10, 0x63},  {}, "Self-Register"},
	
	{ CONSOLE_DISK_BUTTON_1,  		1, HU_ADDRESS, MY_ADDRESS,			CONTROL_FLAGS, 4, {0x00, 0x25, 0x63, 0x80}, {}, "Disk Button 1"},
	{ RESP_DISK_BUTTON_1,			1, MY_ADDRESS, HU_ADDRESS,	 		CONTROL_FLAGS, 5, {0x00, 0x63, 0x12, 0x50, 0x01}, {}, "Disk Response1"},
	{ CONSOLE_DISK_BUTTON_2,  		1, HU_ADDRESS, MY_ADDRESS,			CONTROL_FLAGS, 6, {0x00, 0x11, 0x63, 0x42, 0x01, 0x00}, {}, "Disk Button 2"},
	{ RESP_DISK_BUTTON_2_0,			1, MY_ADDRESS, HU_ADDRESS,	 		CONTROL_FLAGS, 5, {0x00, 0x63, 0x12, 0x52, 0x01}, {}, "Disk Response2-0"},
	{ RESP_DISK_BUTTON_2_1,			0, MY_ADDRESS, HU_ADDRESS,	 		CONTROL_FLAGS, 11, {0x63, 0x31, 0xF1, 0x01, 0x00, 0x80, 0xFF, 0xFF, 0xFF, 0x00, 0x80}, {}, "Disk Response2-1"},
	{ CONSOLE_DISK_PLAY,			0, HU_ADDRESS, LAN_BROADCAST_ADDRESS,	CONTROL_FLAGS, 4, {0x11, 0x01, 0x45, 0x63}, {}, "Console - PLAY"},
	{ RESP_CONSOLE_DISK_PLAY_0,		0, MY_ADDRESS, BROADCAST_ADDRESS,	CONTROL_FLAGS, 11, {0x63, 0x31, 0xF1, 0x01, 0x28, 0x01, 0x01, 0x00, 0x00, 0x00, 0x80}, {}, "Response PLAY0"},
	{ RESP_CONSOLE_DISK_PLAY_1,		0, MY_ADDRESS, BROADCAST_ADDRESS,	CONTROL_FLAGS, 11, {0x63, 0x31, 0xF1, 0x01, 0x10, 0x01, 0x01, 0x00, 0x00, 0x00, 0x80}, {}, "Response PLAY1"},

	{ SELF_PLAYING_STATUS,			0, MY_ADDRESS, BROADCAST_ADDRESS,	CONTROL_FLAGS, 11, {0x63, 0x31, 0xF1, 0x01, 0x10, 0x01, 0x01, 0x00, 0x00, 0x00, 0x80}, {}, "ME: Play Status"},
	
	{ CONSOLE_DISK_STOP, 			1,	HU_ADDRESS, MY_ADDRESS, 		CONTROL_FLAGS, 6, {0x00, 0x11, 0x63, 0x43, 0x01, 0x00}, {}, "Console - Stop" },
	{ RESP_CONSOLE_DISK_STOP_0,		1, MY_ADDRESS, HU_ADDRESS,	CONTROL_FLAGS, 5, {0x00, 0x63, 0x11, 0x53, 0x01 }, {}, "Response STOP0"},
	{ RESP_CONSOLE_DISK_STOP_1,		0, MY_ADDRESS, BROADCAST_ADDRESS,	CONTROL_FLAGS, 11, {0x63, 0x31, 0xF1, 0x00, 0x30, 0x00, 0x00,0x00, 0x00, 0x00, 0x80}, {}, "Response STOP1"},
	
	{ CONSOLE_SEEK_UP,  1, HU_ADDRESS, MY_ADDRESS,						CONTROL_FLAGS, 4, {0x00, 0x25, 0x63, 0x94}, {}, "Seek Up"},
	{ CONSOLE_SEEK_DOWN,  1, HU_ADDRESS, MY_ADDRESS,					CONTROL_FLAGS, 4, {0x00, 0x25, 0x63, 0x95}, {}, "Seek Down"},
	{ CONSOLE_SCAN,  1, HU_ADDRESS, MY_ADDRESS,							CONTROL_FLAGS, 4, {0x00, 0x25, 0x63, 0xA6}, {}, "Scan"},
	{ CONSOLE_BTN_1,  1, HU_ADDRESS, MY_ADDRESS,						CONTROL_FLAGS, 4, {0x00, 0x25, 0x63, 0xB0}, {}, "Button 1"},
	{ CONSOLE_BTN_2,  1, HU_ADDRESS, MY_ADDRESS,						CONTROL_FLAGS, 4, {0x00, 0x25, 0x63, 0xA0}, {}, "Button 2"},
	{ CONSOLE_BTN_3,  1, HU_ADDRESS, MY_ADDRESS,						CONTROL_FLAGS, 4, {0x00, 0x25, 0x63, 0x91}, {}, "Button 3"},
	{ CONSOLE_BTN_4,  1, HU_ADDRESS, MY_ADDRESS,						CONTROL_FLAGS, 4, {0x00, 0x25, 0x63, 0x90}, {}, "Button 4"},
	{ CONSOLE_BTN_5,  1, HU_ADDRESS, MY_ADDRESS,						CONTROL_FLAGS, 4, {0x00, 0x25, 0x63, 0x99}, {}, "Button 5"},
	{ CONSOLE_BTN_6,  1, HU_ADDRESS, MY_ADDRESS,						CONTROL_FLAGS, 4, {0x00, 0x25, 0x63, 0x98}, {}, "Button 6"},
	
};
