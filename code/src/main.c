#include <avr/io.h>
#include <util/delay.h>
#include <avr/pgmspace.h>
#include <avr/interrupt.h> 
#include <avr/eeprom.h>
#include <avr/wdt.h>

#include "task_man.h"
#include "USART.h"
#include "AVC.h"
#include "bluetooth.h"

int main(void)
{
//	wdt_disable();
	_delay_ms(1000);
	init_task_man();
	bluetooth_init();
	USART_INIT_POLLING();
//	USART_INIT_INTERRUPT();
	AVC_init(1);
	sei();
//	usart_put_c('t');
	_delay_ms(500);
	AVC_add_tx_transaction_from_list(SELF_REGISTER);
	sei();
	
	add_task_no_arg(bluetooth_config, 0, SYSTEM_1_S, SYSTEM_1_S);
//	wdt_enable( WDTO_2S );
	
	while(1){
		// Reset watchdog.
//        wdt_reset();
		process_tasks();
	}
	
}

