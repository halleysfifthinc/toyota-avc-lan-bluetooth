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
#ifndef __USART_H
#define __USART_H
 
 
 #define USART_BAUDRATE 9600
 
 #define USART_STATUS   UCSR0A
 #define USART_CONTROL  UCSR0B
 #define USART_DATA     UDR0
 
 #define MAX_TX_BUFFER  355
 #define MAX_RX_BUFFER  50
  
 enum UART_ERROR_CODES {UART_GOOD, UART_BUFFER_FULL, UART_BUFFER_EMPTY, USART_COMP_SHORT, USART_COMP_LONG};
 
 void USART_INIT_INTERRUPT();
 void USART_INIT_POLLING();
 void USART_TRANSMIT_TASK(void);
 void UART_callback_register(no_arg_callback callback, uint8_t escape);
 
 uint8_t usart_put_c(char character);
 uint8_t usart_put_str(char* str);
 uint8_t usart_put_c_str(const char * str);
 uint8_t usart_put_c_arr(char str[]);
 uint8_t usart_put_int(uint16_t integer);
 uint8_t usart_put_int_hex(uint16_t integer);
 uint8_t usart_put_byte(uint8_t byte);
 uint8_t usart_put_byte_hex(uint8_t byte);
 uint8_t usart_put_nl(void);
 uint8_t usart_get_c(void);
 
 uint8_t usart_check_recv(void);
 
 
 #endif