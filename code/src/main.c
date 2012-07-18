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

