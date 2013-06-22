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

#include <avr/io.h>
#include <util/delay.h>
#include <avr/pgmspace.h>
#include <avr/interrupt.h> 
#include <avr/eeprom.h>
#include <avr/wdt.h>

#include "task_man.h"
#include "bluetooth.h"
#include "USART.h"

#define LINE_BUFFER 80
static void bluetooth_connect(void);
static void bluetooth_rec_callback(void);
static uint8_t connected = 0;
static uint8_t is_playing = 0;
static uint8_t pair_search = 0;
static uint8_t pair_count = 0;
static uint8_t pair_index = 0;
static uint8_t tmp_pair_index = 0;
static char line[LINE_BUFFER];
static uint8_t line_index = 0;
static uint8_t callnumber = '0';

uint8_t bluetooth_index()
{
 	uint8_t index = pair_index;
	if(index == 0)
	{
		index = pair_count;
	}
	index--;
	return index;
}

uint8_t bluetooth_is_playing()
{
	return is_playing;
}

char* mac_to_string(char* mac_addr)
{
	static char mac_address[MAC_ADDRESS_STR_LENGTH];
	for(int i = 0; i < MAC_ADDRESS_LENGTH; i++ )
	{
		char val = (mac_addr[i]&0xF0)>>4;
		if(val <= 9)
		{
			mac_address[i*3] = '0' + val;
		}
		else
		{
			mac_address[i*3] = val+('a'-0xA);
		}
		val = mac_addr[i]&0x0F;
		if(val <= 9)
		{
			mac_address[(i*3)+1] = '0' + val;
		}
		else
		{
			mac_address[(i*3)+1] = val+('a'-0xA);
		}
		mac_address[(i*3)+2] = ':';
	}
	mac_address[MAC_ADDRESS_STR_LENGTH-1] = '\0';
	return mac_address;
}

uint8_t bluetooth_connected()
{
	return connected;
}

void bluetooth_init(){

	add_task_no_arg(bluetooth_config, 0, SYSTEM_5_ms, 0);
	add_task_no_arg(bluetooth_rec_callback, SYSTEM_20_ms, SYSTEM_5_ms, 0);
	add_task_no_arg(bluetooth_connect, 10*SYSTEM_1_S, SYSTEM_5_ms, 2*SYSTEM_1_S);
	
}

void reset_pairings()
{
	usart_put_c_str(PSTR("SET BT PAIR *\r\n"));
	bluetooth_config();
	pair_count = 0;
	pair_index = 0;
}

void bluetooth_config(){
	
	usart_put_c_str(PSTR("SET CONTROL ECHO 4\n"));
	usart_put_c_str(PSTR("VOLUME 6\n"));
	usart_put_c_str(PSTR("SET CONTROL CONFIG 100\n"));
	usart_put_c_str(PSTR("SET PROFILE A2DP SINK\n"));
	usart_put_c_str(PSTR("SET PROFILE AVRCP CONTROLLER\n"));
	usart_put_c_str(PSTR("SET BT AUTH * 2323\n"));
	usart_put_c_str(PSTR("SET BT CLASS 200438\n"));
	usart_put_c_str(PSTR("SET BT NAME Matrix\n"));
//	usart_put_c_str(PSTR("SET CONTROL CODEC SBC JOINT_STEREO 48000 0\n"));
//	usart_put_c_str(PSTR("SET CONTROL CODEC SBC JOINT_STEREO 44100 1\n"));
	usart_put_c_str(PSTR("RESET\n"));

}

void bluetooth_next_song(){
	usart_put_c('@');usart_put_c(callnumber);
	usart_put_c_str(PSTR(" AVRCP FORWARD\r\n"));
}

void bluetooth_previous_song(){
	usart_put_c('@');usart_put_c(callnumber);
	usart_put_c_str(PSTR(" AVRCP BACKWARD\r\n"));
}

void bluetooth_pause_song(){
	usart_put_c('@');usart_put_c(callnumber);
	usart_put_c_str(PSTR(" AVRCP PAUSE\r\n"));
}

void bluetooth_play_song(){
	usart_put_c('@');usart_put_c(callnumber);
	usart_put_c_str(PSTR(" AVRCP PLAY\r\n"));
}

void bluetooth_disableSearch()
{
	pair_search = 0;
}

void bluetooth_enableSearch()
{
	pair_search = 1;
}

void bluetooth_connect()
{

	if(!connected && pair_search)
	{
		pair_count = 0;
		tmp_pair_index = 0;
		usart_put_c_str(PSTR("SET BT PAIR\r\n"));
		//usart_put_c_str(PSTR("SET\r\n"));
	}

}

void bluetooth_disconnectAll()
{
	pair_search = 0;
	usart_put_c_str(PSTR("CLOSE 0\r\n"));
	usart_put_c_str(PSTR("CLOSE 1\r\n"));
	usart_put_c_str(PSTR("CLOSE 2\r\n"));
	usart_put_c_str(PSTR("CLOSE 3\r\n"));
	usart_put_c_str(PSTR("CLOSE 4\r\n"));
	usart_put_c_str(PSTR("CLOSE 5\r\n"));
	usart_put_c_str(PSTR("CLOSE 6\r\n"));
	usart_put_c_str(PSTR("CLOSE 7\r\n"));
	usart_put_c_str(PSTR("CLOSE 8\r\n"));
	usart_put_c_str(PSTR("CLOSE 9\r\n"));
	pair_count = 0;
	tmp_pair_index = 0;
	pair_index = 0;
}

void bluetooth_connectIndex(uint8_t index)
{
	bluetooth_disconnectAll();
	pair_index = index;
	usart_put_c_str(PSTR("SET BT PAIR\r\n"));
}

typedef struct lineMatch
{
	uint8_t match;
	uint8_t partial;
	char line[80];
} lineMatch;

lineMatch matchList [] PROGMEM = {
	{MATCHING_PAIR_LIST, 1, "SET BT PAIR \b\b:\b\b:\b\b:\b\b:\b\b:\b\b "},
	{MATCHING_CONNECT, 1, "CONNECT \b AVRCP 17"},
	{MATCHING_CONNECT_ASYNC, 1, "RING \b \b\b:\b\b:\b\b:\b\b:\b\b:\b\b 17 AVRCP"},
	{MATCHING_DISCONNECT, 1, "NO CARRIER \b ERROR 0 "},
	{END_PAIR, 0, "SET"},
	{START_PLAY, 1, "A2DP STREAMING START"},
	{STOP_PLAY, 1, "A2DP STREAMING STOP"},
	{SYNTAX_ERROR, 1, "SYNTAX ERROR"}
};

void parse_line()
{
	
	
char matchEnum = 0xFF;
for(int l = 0; l < sizeof(matchList)/sizeof(lineMatch) && matchEnum == 0xFF; l++)
{
	for(int c = 0; c < 80; c++)
	{
		char character = pgm_read_byte(&(matchList[l].line[c]));
		if( (character == '\0') && (line[c] == character || pgm_read_byte(&(matchList[l].partial))))
		{
			matchEnum = pgm_read_byte(&(matchList[l].match));
			break;
		}
		else if(character == '\b' || line[c] == character)
		{

		}
		else
		{
			break;
		}
	}
	
}

if(matchEnum != 0xFF)
{
	switch(matchEnum){
		case MATCHING_PAIR_LIST:
			if(pair_index == tmp_pair_index && !connected)
			{
				line[12+MAC_ADDRESS_STR_LENGTH] = '\0';
				usart_put_c_str(PSTR("CALL "));
				usart_put_str(&line[12]);
				usart_put_c_str(PSTR("17 AVRCP\r\n"));
				usart_put_c_str(PSTR("CALL "));
				usart_put_str(&line[12]);
				usart_put_c_str(PSTR("19 A2DP\r\n"));
				pair_index++;
				//send connect string
			}
			else
			{
				tmp_pair_index++;
			}
			pair_count++;
		break;
		case MATCHING_CONNECT:
			connected = 1;
			callnumber = line[8];
		break;
		case MATCHING_CONNECT_ASYNC:
			connected = 1;
			callnumber = line[5];
		break;
		case MATCHING_DISCONNECT:
			connected = 0;
			is_playing = 0;
		break;
		case END_PAIR:
			if(pair_index >= pair_count)
			{
				pair_index = 0;
			}
		break;
		case START_PLAY:
			is_playing = 1;
		break;
		case STOP_PLAY:
			is_playing = 0;
		break;
		case SYNTAX_ERROR:
	//		usart_put_c_str(PSTR("SYNTAX_ERROR\r\n"));
		break;
	}
}
else
{
	//usart_put_c('~');
	//usart_put_str(line);
	//usart_put_c_str(PSTR("\r\n"));
}

}


void bluetooth_rec_callback(){

	uint8_t character;
	
	while((character = usart_get_c()) != UART_BUFFER_EMPTY){
		line[line_index] = character;
		if(line_index < LINE_BUFFER-1)
		{
			line_index++;
		}
		if( character == '\n')
		{
			line[line_index-2] = '\0';
			parse_line();
			line_index = 0;
		}
	}

}

