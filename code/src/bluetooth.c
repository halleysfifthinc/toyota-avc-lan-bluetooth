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

static MATCH_STATES match_state = NO_MATCH;
static uint8_t match_place = 0;
static char connected_mac_address[MAC_ADDRESS_LENGTH];
static void bluetooth_rec_callback(void);


void bluetooth_init(){

	add_task_no_arg(bluetooth_rec_callback, SYSTEM_20_ms, SYSTEM_5_ms, 0);
	
}

void bluetooth_config(){
	
	usart_put_c_str(PSTR("SET CONTROL ECHO 4\r\n"));
	usart_put_c_str(PSTR("VOLUME 6\r\n"));
	usart_put_c_str(PSTR("SET CONTROL MICBIAS 10 12\r\n"));
	usart_put_c_str(PSTR("SET CONTROL CONFIG 100\r\n"));
	usart_put_c_str(PSTR("SET PROFILE A2DP SINK\r\n"));
	usart_put_c_str(PSTR("SET PROFILE AVRCP CONTROLLER\r\n"));
	usart_put_c_str(PSTR("SET PROFILE HFP ON\r\n"));
	usart_put_c_str(PSTR("SET BT AUTH * 2323\r\n"));
	usart_put_c_str(PSTR("SET BT CLASS 240428\r\n"));
	usart_put_c_str(PSTR("SET BT NAME Bluetooth Matrix\r\n"));

	usart_put_c_str(PSTR("SET CONTROL AUTOCALL 0017 5000 A2DP\r\n"));

	usart_put_c_str(PSTR("SET CONTROL RINGTONE 6,gfgfgf__gfgfgf______gfgfgf__gfgfgf\r\n"));

	usart_put_c_str(PSTR("RESET\r\n"));

}

void bluetooth_next_song(){
	usart_put_c_str(PSTR("@0 AVRCP FORWARD\r\n"));
}

void bluetooth_previous_song(){
	usart_put_c_str(PSTR("@0 AVRCP BACKWARD\r\n"));
}

void bluetooth_pause_song(){
	usart_put_c_str(PSTR("@0 AVRCP PAUSE\r\n"));
}

void bluetooth_play_song(){
	usart_put_c_str(PSTR("@0 AVRCP PLAY\r\n"));
}

void bluetooth_rec_callback(){

	uint8_t character;
	const char* connect_str = PSTR("CONNECT 0 AVRCP 17\r\n");
	const char* list_str = PSTR("LIST 0 CONNECTED L2CAP 48 0 0 0 0 0 ");
	
	while((character = usart_get_c()) != UART_BUFFER_EMPTY){
		switch(match_state){
		
			case NO_MATCH:
				match_place = 0;
				if( character == pgm_read_byte_near(connect_str)){
					match_state = MATCHING_CONNECT;
					match_place++;
				}
				else if( character == pgm_read_byte_near(list_str)){
					match_state = MATCHING_LIST;
					match_place++;
				}

				break;
			case MATCHING_CONNECT:
				if( character == pgm_read_byte_near(connect_str + match_place)){
					if(character == '\n'){
						match_state = NO_MATCH;
						usart_put_c_str(PSTR("LIST\r\n"));
					}
					else{
						match_place++;
					}
				}
				else{
					match_state = NO_MATCH;
				}
				break;
				
			case MATCHING_LIST:
				{
				uint8_t list_char =  pgm_read_byte_near(list_str + match_place);
				if( character == list_char){
						match_place++;
				}
				else if(list_char == '\0'){
					match_state = FETCHING_MAC;
					connected_mac_address[0] = character;
					match_place = 1;
				}
				else{
					match_state = NO_MATCH;
				}
				break;
				}
			case FETCHING_MAC:
				if(match_place < MAC_ADDRESS_LENGTH-1){
					connected_mac_address[match_place] = character;
					match_place++;
				}
				else{
					connected_mac_address[match_place] = '\0';
					usart_put_c_str(PSTR("CALL "));
					usart_put_str(connected_mac_address);
					usart_put_c_str(PSTR(" 111F HFP\r\n"));
					match_state = NO_MATCH;
				}
				break;
		}
	}

}