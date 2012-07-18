#include <avr/interrupt.h> 
#include <avr/io.h> 
#include <stddef.h>
#include <avr/pgmspace.h>

#include "task_man.h"
#include "USART.h"

 static uint8_t tx_buffer[MAX_TX_BUFFER];
 static uint8_t rx_buffer[MAX_RX_BUFFER];
 
 static uint8_t* tx_buff_head;
 static uint8_t* tx_buff_tail;
 
 static uint8_t* rx_buff_head;
 static uint8_t* rx_buff_tail;

 static no_arg_callback UART_callback = NULL;
 enum TX_TYPE{POLLING, INTERRUPT};
 static enum TX_TYPE  tx_type = POLLING; 
 static uint8_t UART_escape = '\n';

void USART_INIT_INTERRUPT()
{
	tx_type = INTERRUPT;

	//initialize buffers
	tx_buff_head = tx_buffer;
    tx_buff_tail = tx_buffer;
	rx_buff_head = rx_buffer;
    rx_buff_tail = rx_buffer;
	
	// get prescaller value
	uint16_t ubbr = (((F_CPU / (USART_BAUDRATE * 16UL))) - 1);
	
	// Set baud rate
	UBRR0H = (uint8_t)(ubbr>>8);
	UBRR0L = (uint8_t)ubbr;

	// Set frame format to 8 data bits 00s in other fields indicate no parity, 1 stop bit.
	UCSR0C |= (1<<UCSZ01)|(1<<UCSZ00);
	
	// Enable USART Periferal and interrupts
	UCSR0B |= (1<<RXCIE0) | (1<<UDRIE0) | (1 << RXEN0) | (1 << TXEN0);
	
	
}

void USART_INIT_POLLING()
{

	tx_type = POLLING;
	//initialize buffers
	tx_buff_head = tx_buffer;
    tx_buff_tail = tx_buffer;
	rx_buff_head = rx_buffer;
    rx_buff_tail = rx_buffer;
	
	// get prescaller value
	uint16_t ubbr = (((F_CPU / (USART_BAUDRATE * 16UL))) - 1);
	
	// Set baud rate
	UBRR0H = (uint8_t)(ubbr>>8);
	UBRR0L = (uint8_t)ubbr;

	// Set frame format to 8 data bits 00s in other fields indicate no parity, 1 stop bit.
	UCSR0C |= (1<<UCSZ01)|(1<<UCSZ00);
	
	// Enable USART Periferal and interrupts
	UCSR0B = (1<<RXCIE0) | (1 << RXEN0) | (1 << TXEN0);
	
	add_task_no_arg(USART_TRANSMIT_TASK, SYSTEM_2_ms, SYSTEM_5_ms, 0);
}


void UART_callback_register(no_arg_callback callback, uint8_t escape){
	UART_callback = callback;
	UART_escape = escape;
}

//called when data recieve is complete. MUST read from data to clear interrupt flag.
ISR(USART_RX_vect)
{

	uint8_t error;
	uint8_t data;
	uint8_t status;
	
	// read UART status register and UART data register
    status  = USART_STATUS;
    data = USART_DATA;
	
	//fetch any error
	error = (status & ((1<<FE0)|(1<<DOR0)) );
	
	//increment tail
	uint8_t* rx_tail = rx_buff_tail + 1;
	// if at end, reset
	if(rx_tail == rx_buffer + MAX_RX_BUFFER)
		rx_tail = rx_buffer;
		
	//no room left in RX buffer
	if( rx_tail ==  rx_buff_head ){
		return;
	}
	else{
		
		//fill next data slot.
		*(rx_tail) = data;
		rx_buff_tail=rx_tail;
		
		//add callback if exists.
		if(UART_callback != NULL && UART_escape == data){
			add_task_no_arg(UART_callback, 0, 100, 0);
		}
	}	
	
 
}

//called when data is empty.
ISR(USART_UDRE_vect)
{
	//if empty, stop trying to send things.
	if( tx_buff_head ==  tx_buff_tail ){
		USART_CONTROL &= ~(1<<UDRIE0);
	}
	else{
		tx_buff_head++;
		//check to see if at end, if so, reset
		if(tx_buff_head == tx_buffer + MAX_TX_BUFFER)
			tx_buff_head = tx_buffer;
		
		USART_DATA = *tx_buff_head;
		
	}		
 
}

void USART_TRANSMIT_TASK(){
	
//	cli();
	//if empty, stop trying to send things.	
	while( tx_buff_head !=  tx_buff_tail ){
		{
			while(!(UCSR0A & (1<<UDRE0)));
			
			tx_buff_head++;
			//check to see if at end, if so, reset
			if(tx_buff_head == tx_buffer + MAX_TX_BUFFER){
				tx_buff_head = tx_buffer;
			}
			
			USART_DATA = *tx_buff_head;		
		}
	}
//	sei();
}

//This will add a character to the transmit buffer, and send it.
uint8_t usart_put_c(char character){
				
	//increment tail
	uint8_t* tx_tail = ++tx_buff_tail;
	tx_buff_tail--;
	// if at end, reset
	if(tx_tail == tx_buffer + MAX_TX_BUFFER)
		tx_tail = tx_buffer;
		
	//no room left in TX buffer
	if( tx_tail ==  tx_buff_head ){
		return UART_BUFFER_FULL;
	}
	else{
		
		//fill next data slot, THEN increment to avoid race condition
		*(tx_tail) = character;
		tx_buff_tail=tx_tail;
		
		if(tx_type == INTERRUPT){
			// enable UDRE interrupt
			USART_CONTROL |= (1<<UDRIE0);
		}
	}	
	
	
	return UART_GOOD;
}

//This will get a character from the recieve buffer.
uint8_t usart_get_c(){

	//if empty, don't add anything to buffer.
	if( rx_buff_head ==  rx_buff_tail ){
		return UART_BUFFER_EMPTY;
	}
	else{
		uint8_t* rx_head = rx_buff_head + 1;
		//check to see if at end, if so, reset
		if(rx_head == rx_buffer + MAX_RX_BUFFER)
			rx_head = rx_buffer;
		
		uint8_t data = *rx_head;
		rx_buff_head = rx_head;
		
		return data;
	}	
}

//Allows to direct a string as output;
uint8_t usart_put_str(char* str){
	for(; *str != '\0'; str++){
		if(usart_put_c(*str) != UART_GOOD)
			return UART_BUFFER_FULL;
	}
	return UART_GOOD;
}

uint8_t usart_put_c_str(const char * str){

	char c;

	for(; (c = pgm_read_byte_near( str )); str++){
	if(usart_put_c(c) != UART_GOOD)
			return UART_BUFFER_FULL;
	}
	return UART_GOOD;
}


//prints \r\n onto usart bus;
uint8_t usart_put_nl(){
	usart_put_c('\r');
	usart_put_c('\n');
	return UART_GOOD;
}
uint8_t usart_put_int(uint16_t integer){
char values[7];
int8_t i = 0;
	do{	
		uint8_t val = integer%10;
		values[i++] = val;
		integer /= 10;	
	}while(integer);
	do{	
				
		if(usart_put_c('0' + values[--i]) != UART_GOOD)
			return 1;
	}while(i>0);
	return UART_GOOD;
}

uint8_t usart_put_int_hex(uint16_t integer){
	char current = (integer>>12)&0x000F;
	if(current > 0x9)
		usart_put_c(('A'-10)+current);
	else
		usart_put_c('0'+current);
	
	current = (integer>>8)&0x000F;
	if(current > 0x9)
		usart_put_c(('A'-10)+current);
	else
		usart_put_c('0'+current);
	
	current = (integer>>4)&0x000F;
	if(current > 0x9)
		usart_put_c(('A'-10)+current);
	else
		usart_put_c('0'+current);
		
		current = (integer)&0x000F;
	if(current > 0x9)
		usart_put_c(('A'-10)+current);
	else
		usart_put_c('0'+current);
	
	return UART_GOOD;
}

uint8_t usart_put_byte(uint8_t byte){
char values[3];
int8_t i = 0;
	do{	
		uint8_t val = byte%10;
		values[i++] = val;
		byte /= 10;	
	}while(byte);
	do{	
				
		if(usart_put_c('0' + values[--i]) != UART_GOOD)
			return 1;
	}while(i>0);
	return UART_GOOD;
}
uint8_t usart_put_byte_hex(uint8_t byte){
	
	char current = (byte>>4)&0x000F;
	if(current > 0x9)
		usart_put_c(('A'-10)+current);
	else
		usart_put_c('0'+current);
		
		current = (byte)&0x000F;
	if(current > 0x9)
		usart_put_c(('A'-10)+current);
	else
		usart_put_c('0'+current);
	return UART_GOOD;
}

uint8_t usart_check_recv(){

	//if empty, don't add anything to buffer.
	if( rx_buff_head ==  rx_buff_tail ){
		return UART_BUFFER_EMPTY;
	}
	else return UART_GOOD;
}
