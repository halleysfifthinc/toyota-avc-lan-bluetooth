
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