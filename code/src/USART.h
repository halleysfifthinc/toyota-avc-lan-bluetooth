
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