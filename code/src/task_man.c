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
#include <avr/interrupt.h> 
#include <stddef.h>
#include "task_man.h"
#include "USART.h"


static task* task_array[MAX_TASK_CNT];
static task task_array_storage[MAX_TASK_CNT];
static task** task_array_head;
static task** task_array_tail;

static task* task_queue_head;

static volatile uint16_t system_time_keeper = 0;

void init_task_man(){

	for(uint8_t i = 0; i < MAX_TASK_CNT; i++){
		task_array[i] = &(task_array_storage[i]);
		task_array_storage[i].next = NULL;
	}
		
	task_queue_head = NULL;
	task_array_head = task_array;
	task_array_tail = &(task_array[MAX_TASK_CNT-1]);
	
	//setup timer for counting
	TCNT0 = 0; //count is 0;
	OCR0A = F_CPU/64000; //should resolve to 187 counts, or ~1mS
	TCCR0A = 1<<WGM01; // CTC mode
	TCCR0B = (1<<CS01)|(1<<CS00);//(1<<CS21)|(1<<CS20); // clk/32 12Mhz 12000000/64 = 187.5Khz Mhz; 1ms = .001 = x/187500 x = 187
	TIMSK0 = 1<<OCIE0A;
	
}

ISR(TIMER0_COMPA_vect,  ISR_NOBLOCK){
	system_time_keeper++; // add ~1ms to time.
}

uint8_t add_task_no_arg(no_arg_callback function, uint16_t period, uint16_t min_service_time, uint16_t delay){

	//no room left to allocate tasks
	if( task_array_head >  task_array_tail )
		return TASK_ALLOCATOR_FULL;
	else{
		//allocate current task;
		task * cur_task = *task_array_head;
		
		//null the pointer in array to the task;
		*task_array_head = NULL;
		
		//increment to next allocator
		task_array_head++;
		
		//check to see what king of task based on period.
		if(period)
			cur_task->task_type = RECURRING;
		else
			cur_task->task_type = ONE_SHOT;
		
		//set specifics about callback
		cur_task->callback = function;
		
		//initialze remaing members
		cur_task->period = period;
		cur_task->timer = delay;
		cur_task->next = NULL;
		
		//check to ensure we don't need to replace head, if so, do.
		if(task_queue_head == NULL || (task_queue_head->timer > min_service_time && task_queue_head->timer > delay)){
			cur_task->next = task_queue_head;
			task_queue_head = cur_task;
		}
		else{
			//traverse nodes, until we reach a node that excedes our min_service_time or excedes our initial delay.
			task* previous = task_queue_head;
			while(previous->next != NULL && previous->next->timer < min_service_time && ( delay == 0 || previous->next->timer <= delay)){
				previous = previous->next;
			}
			cur_task->next = previous->next;
			previous->next = cur_task;
		}
		
	}	
	
	
	return TASK_GOOD;

}



//we run this every time in our main loop to run tasks
void process_tasks(){

	//we grab the time difference.
	uint16_t time_diff = system_time_keeper;
	//we reset the system timer.
	system_time_keeper = 0;
	
	//only run if difference in time.
	if(time_diff){
		//go through each node to update timer value.
		task* previous = task_queue_head;
		while(previous != NULL){
			if(time_diff > previous->timer)
			{
				previous->timer = 0;
			}
			else{
				previous->timer -= time_diff;
			}
			previous = previous->next;
		}
		
	}
	
	//pop events off queue if ready.
	if(task_queue_head !=NULL && task_queue_head->timer == 0)
	{
		
		task_queue_head->callback();
		switch(task_queue_head->task_type){
			case RECURRING:
				//reset period counter
				task_queue_head->timer = task_queue_head->period;		
				task* previous = task_queue_head;
				task* cur_task = task_queue_head;
				//update task position
				while(previous->next != NULL && previous->next->timer <= task_queue_head->timer){
					previous = previous->next;
				}
				if(previous != task_queue_head){
					task_queue_head = task_queue_head->next;	
					cur_task->next = previous->next;
					previous->next = cur_task;
				}
			break;
			case ONE_SHOT:
				//remove as we ran the task.
				remove_head_task();
			break;
		}
	}
	
}

void remove_head_task(){
	//re-cover task stack place;
	task_array_head--;
	
	//place the task back into stack;
	*task_array_head = task_queue_head;
	
	//update task queue
	task_queue_head = task_queue_head->next;
}