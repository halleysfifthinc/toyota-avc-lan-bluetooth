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


#include <avr/interrupt.h> 
#include <avr/io.h> 
#include <util/delay.h>
#include <stddef.h>
#include <avr/pgmspace.h>
#include <avr/wdt.h>
#include <stdio.h>

#include "task_man.h"
#include "USART.h"
#include "AVC.h"
#include "AVC_MACROS.h"
#include "AVC_messages.h"
#include "bluetooth.h"

extern MSG_IDENTIFIER message_identification_list [];

static void AVC_process_received_messages(void);
static void remove_head_rx_msg(void);
static MSG_ID find_message_id(MESSAGE* msg);

static volatile uint8_t parity = 0;
static volatile uint8_t  broadcast = 0;
static volatile uint16_t master_address = 0;
static volatile uint16_t slave_address = 0;
static volatile uint8_t  control;
static volatile uint8_t  data_size;
static volatile uint8_t  data[ MAX_DATA_BUFFER ];

static volatile uint16_t data_bits = 0;
static volatile uint8_t bit_count = 0;
static volatile uint16_t bit_mask = 0;
static volatile uint8_t  data_count = 0;

static volatile AVC_FRAMES bit_frame = IDLE;

static MESSAGE message_tx_buffer[MAX_MESSAGE_CNT_TX];
static MESSAGE message_rx_buffer[MAX_MESSAGE_CNT_RX];

static MESSAGE* message_tx_head;//pop here
static MESSAGE* message_tx_tail;//insert here
static MESSAGE* message_rx_head;//pop here
static MESSAGE* message_rx_tail;//insert here


static uint8_t usart_quiet = 0;

static uint8_t IS_PLAYING = 0;
static uint8_t SECONDS = 0;
static uint8_t MINUTES = 0;

void status_update(void){
	if(IS_PLAYING){
		MESSAGE* msg = AVC_add_tx_transaction_from_list(SELF_PLAYING_STATUS);
		msg->data[8] = AVC_Toyota_digits(SECONDS++);
		msg->data[7] = AVC_Toyota_digits(MINUTES);
		sei();
		if(SECONDS == 60){
			SECONDS = 0;
			MINUTES++;
		}
		if(MINUTES == 60){
			MINUTES = 0;
		}
		
	}
}

void AVC_init(uint8_t quiet){
	usart_quiet = quiet;
	
	message_tx_head = message_tx_buffer;
	message_tx_tail = message_tx_head;
	message_rx_head = message_rx_buffer;
	message_rx_tail = message_rx_head;

	add_task_no_arg(AVC_process_received_messages, SYSTEM_20_ms, SYSTEM_5_ms, 0);

	AVC_DDR &= ~(1<<RI);
	AVC_DDR |= (1<<TO) | (1<<RE) | (1<<TE);
	AVC_PORT &= ~( (1<<RI) | (1<<TO) | (1<<RE) | (1<<TE) ); 


	//setup timer for avc communication 16 bits are
	// needed to keep best resolution and full scale of data.
	TCNT1 = 0; //count is 0;
	TCCR1A = 0; // Normal mode
	TCCR1B = 1<<CS10; // NO PRESCALLING	
	
	EICRA = (1<<ISC00); //any change triggeres intterupt
	EIMSK = (1<<INT0); //enable pin interrupt INT0 
	
	TCNT1 = 0; //count is 0;
	OCR1A = LINE_BUSY_CHECK;

	TIMSK1 |= (1<<OCIE1A); //enable send ISR to watch for free/busy line
	
	add_task_no_arg(status_update, SYSTEM_1_S, 0, 0);
}



MESSAGE* AVC_add_tx_transaction_from_list(MSG_ID id){
	MESSAGE* msg;
	while((msg = AVC_add_tx_transaction( pgm_read_word(&(message_identification_list[id].slave)), 
							pgm_read_byte(&(message_identification_list[id].broadcast)),
							pgm_read_byte(&(message_identification_list[id].control)),
							pgm_read_byte(&(message_identification_list[id].data_size)),
							message_identification_list[id].data, 0) ) == NULL) {sei();}
	if(!usart_quiet){
		usart_put_c_str( PSTR("-->\r\n"));
		usart_put_c_str( message_identification_list[id].description);
		usart_put_c_str( PSTR("\r\n"));
	}
	return msg;
}
MESSAGE* AVC_add_tx_transaction(uint16_t address_arg, uint8_t broadcast_arg, uint8_t control_arg, uint8_t data_size_arg, uint8_t * data_arg, uint8_t print){

	cli(); //we don't want to accidentaly update trasaction head AND tail at the same time.
//no room left to allocate tasks
	MESSAGE* tx_tail = ++message_tx_tail;
	message_tx_tail--;
	
	// if at end, reset
	if(tx_tail == message_tx_buffer + MAX_MESSAGE_CNT_TX)
		tx_tail = message_tx_buffer;
		
	//no room left in TX buffer
	if( tx_tail ==  message_tx_head ){
		sei();
		return NULL;
	}
	else{
		//fetch current item.
		sei(); //enable interupts again. while we do things here... (tail will still be at old place so data won't be sent prematurely)		
				
		message_tx_tail->master_address = MY_ADDRESS;
		message_tx_tail->slave_address = address_arg;
		message_tx_tail->broadcast = broadcast_arg;
		message_tx_tail->control = control_arg;
		message_tx_tail->data_size = data_size_arg;
		
		while(data_size_arg){
			data_size_arg--;
			message_tx_tail->data[data_size_arg] = pgm_read_byte(&(data_arg[data_size_arg]));
		}
		
		if(print && !usart_quiet){
			usart_put_c_str( PSTR("-->\r\n"));
			usart_put_c_str( PSTR("B: "));
			usart_put_byte(broadcast_arg);
			
			usart_put_c_str( PSTR("\r\nM: "));
			usart_put_int_hex(MY_ADDRESS);
			
			usart_put_c_str( PSTR("\r\nS: "));
			usart_put_int_hex(address_arg);
			
			usart_put_c_str( PSTR("\r\nC: "));
			usart_put_byte_hex(control_arg);
			
			usart_put_c_str( PSTR("\r\nL: "));
			uint8_t data_s = data_size_arg;
			usart_put_byte(data_s);
			usart_put_c_str( PSTR("\r\nD: "));
			uint8_t* data_ptr = message_tx_tail->data;
			if(data_s > MAX_DATA_BUFFER)
				data_s = MAX_DATA_BUFFER;
			while(data_s > 0 ){
				data_s--;
				usart_put_byte_hex(*data_ptr);
				usart_put_c(' ');
				data_ptr++;
			}
			usart_put_c_str( PSTR("\r\n\r\n"));
		}
		
		cli();
		//save for return value
		MESSAGE* msg_ptr = message_tx_tail;
		//increment tail to new.
		message_tx_tail= tx_tail;
		//**NOTE**
		//we don't enable intterrupt as we MAY want to edit data quickly. DON'T forget to re-enable them!!!
	return msg_ptr;
	}
}
static MSG_ID find_message_id(MESSAGE* msg){
	for(MSG_ID msg_id = 0; msg_id < MESSAGE_ID_SIZE; msg_id++){
		//match master, slave, broadcast, control, and data length.
		if( (msg->broadcast == pgm_read_byte(&(message_identification_list[msg_id].broadcast)) ) &&
			(msg->master_address == pgm_read_word(&(message_identification_list[msg_id].master))) &&
			(msg->slave_address == pgm_read_word(&(message_identification_list[msg_id].slave)) ) &&
			(msg->control == pgm_read_byte(&(message_identification_list[msg_id].control)) ) &&
			(msg->data_size == pgm_read_byte(&(message_identification_list[msg_id].data_size)) )){
			uint8_t match = 1;
			for(int i = 0; i < msg->data_size; i++){
				if(pgm_read_byte(&(message_identification_list[msg_id].data_ignore[i])) || msg->data[i] == pgm_read_byte(&(message_identification_list[msg_id].data[i])) )
				{}
				else
				{
					match = 0;
					break;
				}
			}
			
			if(match){
				return msg_id;
			}
		}
	}

	return MESSAGE_ID_SIZE;
}

void handle_message(MSG_ID id){

	switch(id){
		case CD_CH_LAN_CHECK:
			{
				MESSAGE* message;
				while( (message = AVC_add_tx_transaction_from_list(RESP_LAN_CHECK)) == NULL){
					sei();//release while we don't have something.
				}
					message->data[4] = message_rx_head->data[4];
					sei();//release once we have altered what we need to.
			}
		break;
		case CONSOLE_DISK_BUTTON_1:
			{
				MESSAGE* message;
				while( (message = AVC_add_tx_transaction_from_list(RESP_DISK_BUTTON_1)) == NULL){
					sei();
				}
					sei();
			}
			break;
		case CONSOLE_DISK_BUTTON_2:
			{
				MESSAGE* message;
				while( (message = AVC_add_tx_transaction_from_list(RESP_DISK_BUTTON_2_0)) == NULL){
					sei();
				}
					sei();
				while( (message = AVC_add_tx_transaction_from_list(RESP_DISK_BUTTON_2_1)) == NULL){
					sei();
				}
					sei();
			}
			break;
		case CONSOLE_DISK_PLAY:
			{
				if(!IS_PLAYING){
					MESSAGE* message;
					while( (message = AVC_add_tx_transaction_from_list(RESP_CONSOLE_DISK_PLAY_0)) == NULL){
						sei();
					}
						sei();
					while( (message = AVC_add_tx_transaction_from_list(RESP_CONSOLE_DISK_PLAY_1)) == NULL){
						sei();
					}
					sei();
					
					SECONDS = 0;
					MINUTES = 0;
					IS_PLAYING = 1;
					bluetooth_play_song();
				}
				
			}
			break;
		case CONSOLE_DISK_STOP:
			{
				if(IS_PLAYING){
					MESSAGE* message;
					while( (message = AVC_add_tx_transaction_from_list(RESP_CONSOLE_DISK_STOP_0)) == NULL){
						sei();
					}
						sei();
					while( (message = AVC_add_tx_transaction_from_list(RESP_CONSOLE_DISK_STOP_1)) == NULL){
						sei();
					}
						sei();
					IS_PLAYING = 0;
					bluetooth_pause_song();
				}
				
			}
			break;
		case CONSOLE_SEEK_UP:
			{
				if(IS_PLAYING){
					bluetooth_next_song();
				}
			}
			break;
		case CONSOLE_SEEK_DOWN:
			{
				if(IS_PLAYING){
					bluetooth_previous_song();
				}
			}
		default:
		break;
	}

}

void AVC_process_received_messages(){
	if(MSG_RECEIVED)
	{
		MSG_ID id = find_message_id(message_rx_head);
		
		if(id == MESSAGE_ID_SIZE && !usart_quiet){
			usart_put_c_str( PSTR("<--\r\n"));
			usart_put_c_str( PSTR("B: "));
			usart_put_byte(message_rx_head->broadcast);
			
			usart_put_c_str( PSTR("\r\nM: "));
			usart_put_int_hex(message_rx_head->master_address);
			
			usart_put_c_str( PSTR("\r\nS: "));
			usart_put_int_hex(message_rx_head->slave_address);
			
			usart_put_c_str( PSTR("\r\nC: "));
			usart_put_byte_hex(message_rx_head->control);
			
			usart_put_c_str( PSTR("\r\nL: "));
			uint8_t data_s = message_rx_head->data_size;
			usart_put_byte(data_s);
			usart_put_c_str( PSTR("\r\nD: "));
			uint8_t* data_ptr = message_rx_head->data;
			if(data_s > MAX_DATA_BUFFER)
				data_s = MAX_DATA_BUFFER;
			while(data_s > 0 ){
				data_s--;
				usart_put_byte_hex(*data_ptr);
				usart_put_c(' ');
				data_ptr++;
			}
			usart_put_c_str( PSTR("\r\n\r\n"));
		}
		else{
			if(!usart_quiet){
				usart_put_c_str( PSTR("<--\r\n"));
				usart_put_c_str( PSTR("MSG: "));
				usart_put_c_str( message_identification_list[id].description);
				usart_put_c_str( PSTR("\r\n"));
			}
			handle_message(id);
		}

		remove_head_rx_msg();
	}
}

void remove_head_rx_msg(){
	//re-cover msg stack place;
	cli();
	message_rx_head++;
	if(message_rx_head == message_rx_buffer + MAX_MESSAGE_CNT_RX)
		message_rx_head = message_rx_buffer;
	sei();
}


uint8_t AVC_Toyota_digits(uint8_t num){
	uint8_t hex = 0;
	hex |= num%10;
	hex |= (num/10)<<4;
	return hex;
}

/* The way this works, is via a well-defined switch case statement. 
** Data is placed and sensed via the external interrupt pint on the 
** microcontroller. When it is pulsed, if it is a rising edge, it triggers
** data capture. I did make an attempt at "outside of ISR" handling, 
** but the requests just became unsynchronised and parity mismatch exploded.
**
** 		**********BUG LIST********
** 	) For some reason, During one of the DATA_PAYLOAD instances, the parity
**	  check fails. I have yet to determine why, but it is not frequent enough
** 	  to stop development entirely. I suspect it is just timing of data that 
**	  could be tweaked a bit. 
**		*****FIX????: Turns out that this code is VERY time sensitive. It comes
**      so close to being able to catch the bits in time, that I had to take care
**      of (remove or make no_block) other ISRs to ensure the code could catch 
**      the bits in time. USART driver is now polling based for transmit, and
**      utilizes the new task queue to do it's job. The timer for task_man is now
**		no_block, which will mean that the timer could be off by up to several ms.
**		The ONLY way I could see a better way would be to do this is with a faster 
**      osclillator :)
*/

ISR(INT0_vect){
	uint16_t count = TCNT1; //grab timer value
	RESET_TIMER(); //reset timer for value checking
	
	if(REC_HIGH){
		//insert room for bit, add to counter
		data_bits <<=1;
		
		if(FRAME_IS_IDLE && count > START_BIT_HOLD_LOW_CHECK ){
			data_bits = 1;
			bit_frame = START;
		}
		else if(count < BIT_POLL_LENGTH)
		{
			// Set new bit.
			data_bits |= 0x0001;
			parity = !parity;
		}

//		if(!FRAME_IS_ERROR && !FRAME_IS_FINISHED && !FRAME_IS_IDLE)
		
		switch(bit_frame){
			case IDLE:
				break;
			case START:
				FRAMING_RESET();
				FRAMING_NEXT();
				break;
			case BROADCAST:
				broadcast = data_bits;
				FRAMING_RESET();
				FRAMING_NEXT();
				break;
			case MASTER_ADDR:
				master_address = data_bits>>1;
				FRAMING_NEXT();
				FRAMING_PARITY_CHECK();
				FRAMING_RESET();
				break;
			case SLAVE_ADDR:
				slave_address = data_bits>>1;
				FRAMING_NEXT();
				FRAMING_PARITY_CHECK();
				FRAMING_RESET();
				if(FRAMING_FOR_ME){
					FRAMING_SEND_ACK();
					FRAMING_NEXT();
				}
				break;
			case SLAVE_ACK:
				FRAMING_RESET();
				FRAMING_NEXT();
				break;
			case CONTROL:
				control = data_bits>>1;
				FRAMING_NEXT();
				FRAMING_PARITY_CHECK();
				FRAMING_RESET();
				if(FRAMING_FOR_ME){
					FRAMING_SEND_ACK();
					FRAMING_NEXT();
				}
				break;
			case CONTROL_ACK:
				FRAMING_RESET();
				FRAMING_NEXT();
				break;
			case DATA_SIZE:
				data_size = data_bits>>1;
				FRAMING_NEXT();
				FRAMING_PARITY_CHECK();
				FRAMING_READY_DATA();
				FRAMING_RESET();
				if(FRAMING_FOR_ME){
					FRAMING_SEND_ACK();
					FRAMING_NEXT();
				}
				break;
			case DATA_SIZE_ACK:
				FRAMING_RESET();
				FRAMING_DATA_ACK_CHECK();
				break;
			case DATA_PAYLOAD:
				data[data_count++] = data_bits>>1;
				FRAMING_NEXT();
				FRAMING_PARITY_CHECK();
				FRAMING_RESET();
				if(FRAMING_FOR_ME){
					FRAMING_SEND_ACK();
					FRAMING_DATA_ACK_CHECK();
				}
				break;
			case DATA_PAYLOAD_ACK:
				FRAMING_RESET();
				FRAMING_DATA_ACK_CHECK(); //end of line, so we use DATA_SIZE_ACK+1.
				break;
			case FRAME_FINISHED:
				break;
			case FRAME_ERROR:
				break;
			default:
				FRAMING_NEXT();
				break;
		
		}
	
	}
	else{		
		
	}
	
}


/* We will try periodlically checking the bus to see if it is active
** and if there are any outstanding transactions that need to be sent.
** If so, we will try to use the same ISR to both detect and send data
** by altering the OCR1A register to accomadate.
*/
ISR(TIMER1_COMPA_vect){
	DISABLE_PIN_CHANGE();
	
	//if we got here initially, bit_frame should be in IDLE state. Good to check anyways to be sure!
	if(FRAME_IS_IDLE && READY_TO_TRANSMIT){
		LOAD_GLOBALS(message_tx_head);
		
		SEND_START_BIT();
		
		SEND_BROADCAST_BIT();
		
		//master address
		SEND_ADDRESS(master_address);
		
		SEND_PARITY();
		
		SEND_ADDRESS(slave_address);
		
		SEND_PARITY();

		//ACK
		ACKNOWLEDGE();
		
		//control
		SEND_CONTROL();
		
		//parity bit
		SEND_PARITY();
		
		//ACK
		ACKNOWLEDGE();
		
		//data Size
		SEND_BYTE(data_size);
		
		//parity bit
		SEND_PARITY();
		
		//ACK
		ACKNOWLEDGE();
		
		//send all our data.
		data_count = 0;
		while(data_count < data_size){
			
			uint8_t data_to_send = data[data_count++];
			SEND_BYTE(data_to_send);
			
			//parity bit
			SEND_PARITY();
			
			//ACK
			ACKNOWLEDGE();
		}
		
		REMOVE_HEAD_TX_MSG();
				
	}

	ENABLE_PIN_CHANGE();
	RESET_TIMER();
	
}

//This ISR for the COMPB value will be used to send the ACK needed for transmission.
