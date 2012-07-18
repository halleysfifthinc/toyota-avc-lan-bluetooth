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

#ifndef __TASK_MAN_H
#define __TASK_MAN_H
 
 
#define SYSTEM_1_S  1002
#define SYSTEM_5_ms 5
#define SYSTEM_2_ms 2
#define SYSTEM_20_ms 20

#define MAX_TASK_CNT 15
 
typedef void (*no_arg_callback)(void);

 
typedef enum TASK_TYPE {ONE_SHOT, RECURRING} TASK_TYPE; 
enum TASK_ERROR_CODES {TASK_GOOD, TASK_ALLOCATOR_FULL};
 
typedef struct task{
	TASK_TYPE task_type;
	no_arg_callback callback;
	uint16_t period;
	uint16_t timer;
	
	struct task* next;
}task;

void init_task_man(void);
uint8_t add_task_no_arg(no_arg_callback, uint16_t period, uint16_t min_service_time, uint16_t delay);
void process_tasks(void);
void remove_head_task(void);

#endif