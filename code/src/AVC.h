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

#ifndef __AVC_OTHER_H
#define __AVC_OTHER_H
#include "AVC_messages.h"

#define MAX_DATA_BUFFER             20
#define MAX_MESSAGE_CNT_TX			5 //we *SHOULD* only ever queue a handful of messages at any given time.
										//using the function provided, it will even stall (in main runtime) until addition successful
#define MAX_MESSAGE_CNT_RX			6 //This is also quite generous, as we have seen that it takes MANY messages to reach this limit. 

#define AVC_DDR DDRD
#define AVC_PORT PORTD
#define AVC_PIN PIND
#define RI 2
#define TO 3
#define RE 4
#define TE 5 

#define NORMAL_BIT_LENGTH           (12*40) //
#define BIT_1_HOLD_LOW_LENGTH        (12*20) // 240
#define BIT_0_HOLD_LOW_LENGTH        (12*32) // 370
#define BIT_1_HOLD_HIGH_LENGTH       (NORMAL_BIT_LENGTH - BIT_1_HOLD_LOW_LENGTH)
#define BIT_0_HOLD_HIGH_LENGTH       (NORMAL_BIT_LENGTH - BIT_0_HOLD_LOW_LENGTH)
#define BIT_POLL_LENGTH				(BIT_0_HOLD_LOW_LENGTH - ((BIT_0_HOLD_LOW_LENGTH - BIT_1_HOLD_LOW_LENGTH) / 2))

#define ACK_HOLD_TIME				(BIT_1_HOLD_LOW_LENGTH)
#define ACK_CHECK_LENGTH			((BIT_0_HOLD_LOW_LENGTH - ((BIT_0_HOLD_LOW_LENGTH - BIT_1_HOLD_LOW_LENGTH) / 2)) - BIT_1_HOLD_LOW_LENGTH)
#define ACK_TIMEOUT                 (25*12)

#define START_BIT_LENGTH            (12*186)
#define START_BIT_HOLD_LOW_LENGTH    (12*168)
#define START_BIT_HOLD_LOW_CHECK     (12*160)
#define LINE_BUSY_CHECK				(12*200)

//AVC framing information to aid the receive parsing.
typedef enum avc_frames{ 	IDLE = 0,
							START = 1,
							BROADCAST = 2,
							MASTER_ADDR_S = 14,
							MASTER_ADDR = 15,
							SLAVE_ADDR_S = 27,
							SLAVE_ADDR = 28,
							SLAVE_ACK = 29,
							CONTROL_S = 33,
							CONTROL = 34,
							CONTROL_ACK = 35,
							DATA_SIZE_S = 43,
							DATA_SIZE = 44,
							DATA_SIZE_ACK = 45,
							DATA_PAYLOAD_S = 53,
							DATA_PAYLOAD = 54,
							DATA_PAYLOAD_ACK = 55,
							FRAME_FINISHED = 56,
							FRAME_ERROR = 57,
							} AVC_FRAMES;
		

//AVC message structure for received messages, and those to be transmitted.
typedef struct message{
	uint16_t master_address;
	uint16_t slave_address;
	uint8_t broadcast;
	uint8_t control;
	uint8_t data_size;
	uint8_t data[MAX_DATA_BUFFER];
} MESSAGE;






void AVC_init(uint8_t quiet);
MESSAGE* AVC_add_tx_transaction(uint16_t address_arg, uint8_t broadcast_arg, uint8_t control_arg, uint8_t data_size_arg, uint8_t * data_arg, uint8_t print);
MESSAGE* AVC_add_tx_transaction_from_list(MSG_ID id);
uint8_t AVC_Toyota_digits(uint8_t num);
#endif