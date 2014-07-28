#ifndef _TASK_H
#define _TASK_H

#define NO_TASKS	15 /* Number of identical tasks */

#define USB_TASK_PRIO		2
#define START_TASK_PRIO		4
#define KEYBOARD_TASK_PRIO	6
#define UPDATE_TASK_PRIO	7
#define CLEAN_TASK_PRIO		8	/* priority must higher than parser */
#define PARSER_TASK_PRIO	10
#define STATUS_TASK_PRIO	11	/* Can not do anything ocupy time */
#define CONTROL_TASK_PRIO	12
#define CALI_TASK_PRIO		14
#define MOTION_TASK_PRIO	5

#define USB_TASK_ID		102
#define START_TASK_ID		104
#define KEYBOARD_TASK_ID	105
#define UPDATE_TASK_ID		106
#define CLEAN_TASK_ID		108
#define PARSER_TASK_ID		110
#define STATUS_TASK_ID		111
#define CONTROL_TASK_ID		112
#define CALI_TASK_ID		114
#define MOTION_TASK_ID		107

#define START_TASK_STK_SIZE	2048
#define USB_TASK_STK_SIZE	512
#define STATUS_TASK_STK_SIZE	512
#define CLEAN_TASK_STK_SIZE	512
#define PARSER_TASK_STK_SIZE	2048
#define CONTROL_TASK_STK_SIZE	512
#define KEYBOARD_TASK_STK_SIZE	512
#define UPDATE_TASK_STK_SIZE	256
#define CALI_TASK_STK_SIZE	512
#define MOTION_TASK_STK_SIZE	512

void status_Init(void);
void status_Task(void* data);

INT8U USB_Init(void);
void USB_Task(void* data);

void Parser_Task(void *data);
void Update_Task (void *data);
void Clean_Task(void *data);

void Control_Task (void *data);

void Keyboard_Task(void *data);

#endif
