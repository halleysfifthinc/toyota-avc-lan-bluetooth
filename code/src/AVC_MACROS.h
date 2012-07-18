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

#ifndef __AVC_MACROS_H
#define __AVC_MACROS_H

#define EN_RECEIVE(){ AVC_PORT &= ~((1<<TE) | (1<<RE)); }
#define EN_TRANSMIT(){ AVC_PORT |= (1<<TE); }
#define DISABLE_TRANSMIT(){ AVC_PORT &= ~(1<<TE); }
#define TRANSMIT_LOW() { AVC_PORT &= ~(1<<TO); EN_TRANSMIT();}
#define TRANSMIT_HIGH() { DISABLE_TRANSMIT();}
#define INPUT_IS_CLEAR (AVC_PIN&(1<<RI))
#define INPUT_IS_SET !(AVC_PIN&(1<<RI))
#define REC_HIGH (AVC_PIN&(1<<RI))
#define REC_LOW !(AVC_PIN&(1<<RI))

/********************************RECEIVE MACROS***************************/

#define MSG_RECEIVED ( message_rx_head != message_rx_tail )

#define FRAMING_FOR_ME (MY_ADDRESS == slave_address) 
#define FRAME_IS_IDLE (bit_frame == IDLE)

#define FRAMING_ERROR_HANDLER() { if(!usart_quiet){usart_put_c_str(PSTR("Error: ")); usart_put_byte(bit_frame); usart_put_c_str(PSTR("[")); usart_put_byte_hex(control); usart_put_c_str(PSTR("]\r\n"));} FRAMING_RESTART(); return;}
#define FRAMING_FINISH_HANDLER() { ADD_RECEIVED_TRANSACTION(); FRAMING_RESTART(); return;}

#define FRAMING_RESET() {data_bits = 0; parity = 0;}
#define FRAMING_NEXT() {bit_frame++;}
#define FRAMING_PARITY_CHECK() {if(parity){FRAMING_ERROR_HANDLER();}}
#define FRAMING_READY_DATA() {data_count = 0;}
#define FRAMING_DATA_ACK_CHECK() {if(data_count == data_size){ bit_frame = FRAME_FINISHED; FRAMING_FINISH_HANDLER(); }else{ bit_frame = DATA_SIZE_ACK+1;}}
#define FRAMING_SEND_ACK() { \
						 TCNT1 = 0; \
						 while (INPUT_IS_CLEAR)	{ \
							if (TCNT1 >= ACK_TIMEOUT) return; \
						 }\
						 TCNT1 = 0;\
						 TRANSMIT_LOW();\
						 while(TCNT1 < BIT_0_HOLD_LOW_LENGTH);	\
						 TRANSMIT_HIGH();	\
						}

#define FRAMING_RESTART() { bit_frame = IDLE;}
#define RESET_TIMER() { TCNT1 = 0;}


#define ADD_RECEIVED_TRANSACTION() {\
	MESSAGE* rx_tail = ++message_rx_tail;\
	message_rx_tail--;\
	if(rx_tail == message_rx_buffer + MAX_MESSAGE_CNT_RX)\
		rx_tail = message_rx_buffer;\
		\
	if( rx_tail ==  message_rx_head ){\
		sei();\
		return;\
	}\
	else{\
		\
		message_rx_tail->master_address = master_address;\
		message_rx_tail->slave_address = slave_address;\
		message_rx_tail->broadcast = broadcast;\
		message_rx_tail->control = control;\
		\
		if(data_size > MAX_DATA_BUFFER)\
			data_size = MAX_DATA_BUFFER;\
		message_rx_tail->data_size = data_size;\
		\
		while(data_size > 0){\
			data_size--;\
			message_rx_tail->data[data_size] = data[data_size];\
		}\
		message_rx_tail = rx_tail;\
	}\
}		


/**********************SEND MACROS***********************/

#define READY_TO_TRANSMIT ( message_tx_head != message_tx_tail )

#define LOAD_GLOBALS(msg){\
    broadcast = msg->broadcast;\
    master_address =  msg->master_address;\
    slave_address = msg->slave_address;\
	control = msg->control;\
    data_size = msg->data_size ;\
    for ( uint8_t i = 0; i < data_size; i++ )\
    {\
        data[i] = msg->data[i] ;\
    }}

#define DISABLE_PIN_CHANGE() {EIMSK &= ~(1<<INT0);}
#define ENABLE_PIN_CHANGE()  {EIMSK |= (1<<INT0);}

#define SEND_START_BIT() {\
		RESET_TIMER();\
		TRANSMIT_LOW();\
		while(TCNT1 < START_BIT_HOLD_LOW_LENGTH);\
		TRANSMIT_HIGH();\
		while(TCNT1 < START_BIT_LENGTH);}
		
#define SEND_BROADCAST_BIT(){\
		TRANSMIT_LOW();\
		RESET_TIMER();\
		if(broadcast)\
			while(TCNT1 < BIT_1_HOLD_LOW_LENGTH);\
		else\
			while(TCNT1 < BIT_0_HOLD_LOW_LENGTH);\
		TRANSMIT_HIGH();\
		while(TCNT1 < NORMAL_BIT_LENGTH);}

#define SEND_ADDRESS(address){\
		bit_mask = (1<<11);\
		parity = 0;\
		while(bit_mask){\
			TRANSMIT_LOW();\
			RESET_TIMER();\
			if(address&bit_mask){\
				parity = !parity;\
				while(TCNT1 < BIT_1_HOLD_LOW_LENGTH);\
			}\
			else\
				while(TCNT1 < BIT_0_HOLD_LOW_LENGTH);\
			TRANSMIT_HIGH();\
			bit_mask>>=1;\
			while(TCNT1 < NORMAL_BIT_LENGTH);\
		}}
		
#define SEND_CONTROL(){\
		bit_mask = (1<<3);\
		parity = 0;\
		while(bit_mask){\
			TRANSMIT_LOW();\
			RESET_TIMER();\
			if(control&bit_mask){\
				parity = !parity;\
				while(TCNT1 < BIT_1_HOLD_LOW_LENGTH);\
			}\
			else\
				while(TCNT1 < BIT_0_HOLD_LOW_LENGTH);\
			TRANSMIT_HIGH();\
			bit_mask>>=1;\
			while(TCNT1 < NORMAL_BIT_LENGTH);\
		}}

#define SEND_BYTE(x){\
		bit_mask = (1<<7);\
		parity = 0;\
		while(bit_mask){\
			TRANSMIT_LOW();\
			RESET_TIMER();\
			if(x&bit_mask){\
				parity = !parity;\
				while(TCNT1 < BIT_1_HOLD_LOW_LENGTH);\
			}\
			else\
				while(TCNT1 < BIT_0_HOLD_LOW_LENGTH);\
			TRANSMIT_HIGH();\
			bit_mask>>=1;\
			while(TCNT1 < NORMAL_BIT_LENGTH);\
		}}
		
#define SEND_PARITY(){\
		TRANSMIT_LOW();\
		RESET_TIMER();\
		if(parity)\
			while(TCNT1 < BIT_1_HOLD_LOW_LENGTH);\
		else\
			while(TCNT1 < BIT_0_HOLD_LOW_LENGTH);\
		TRANSMIT_HIGH();\
		while(TCNT1 < NORMAL_BIT_LENGTH);\
		}

#define ACKNOWLEDGE() {\
		if (broadcast)\
		{\
			RESET_TIMER();\
			TRANSMIT_LOW();\
			while ( TCNT1 < BIT_1_HOLD_LOW_LENGTH );\
			TRANSMIT_HIGH();\
			while ( INPUT_IS_SET );\
			if ( TCNT1 > BIT_POLL_LENGTH)\
			{\
				while ( INPUT_IS_SET );\
			}\
			else{\
				RESET_TIMER();\
				OCR1A = LINE_BUSY_CHECK;\
				ENABLE_PIN_CHANGE();\
				if(!usart_quiet)\
					usart_put_c_str(PSTR("ACK ERROR!\r\n"));\
				return;\
			}\
		}\
		else{\
			RESET_TIMER();\
			TRANSMIT_LOW();\
			while(TCNT1 < BIT_0_HOLD_LOW_LENGTH);\
			TRANSMIT_HIGH();\
			while(TCNT1 < NORMAL_BIT_LENGTH);\
		}}\
		
#define REMOVE_HEAD_TX_MSG() {	\
	message_tx_head++;\
	if(message_tx_head == message_tx_buffer + MAX_MESSAGE_CNT_TX)\
		message_tx_head = message_tx_buffer;}

#endif