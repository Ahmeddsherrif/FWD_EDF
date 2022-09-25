/*
 * FreeRTOS Kernel V10.2.0
 * Copyright (C) 2019 Amazon.com, Inc. or its affiliates.  All Rights Reserved.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy of
 * this software and associated documentation files (the "Software"), to deal in
 * the Software without restriction, including without limitation the rights to
 * use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of
 * the Software, and to permit persons to whom the Software is furnished to do so,
 * subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
 * FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
 * COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
 * IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 *
 * http://www.FreeRTOS.org
 * http://aws.amazon.com/freertos
 *
 * 1 tab == 4 spaces!
 */

/* 
	NOTE : Tasks run in system mode and the scheduler runs in Supervisor mode.
	The processor MUST be in supervisor mode when vTaskStartScheduler is 
	called.  The demo applications included in the FreeRTOS.org download switch
	to supervisor mode prior to main being called.  If you are not using one of
	these demo application projects then ensure Supervisor mode is used.
*/


/*
 * Creates all the demo application tasks, then starts the scheduler.  The WEB
 * documentation provides more details of the demo application tasks.
 * 
 * Main.c also creates a task called "Check".  This only executes every three 
 * seconds but has the highest priority so is guaranteed to get processor time.  
 * Its main function is to check that all the other tasks are still operational.
 * Each task (other than the "flash" tasks) maintains a unique count that is 
 * incremented each time the task successfully completes its function.  Should 
 * any error occur within such a task the count is permanently halted.  The 
 * check task inspects the count of each task to ensure it has changed since
 * the last time the check task executed.  If all the count variables have 
 * changed all the tasks are still executing error free, and the check task
 * toggles the onboard LED.  Should any task contain an error at any time 
 * the LED toggle rate will change from 3 seconds to 500ms.
 *
 */

/* Standard includes. */
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>

/* Scheduler includes. */
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "lpc21xx.h"

/* Peripheral includes. */
#include "serial.h"
#include "GPIO.h"


#define NUMBER_OF_TASKS				((uint8_t)6)	
						
#define PROBE_TICK						PIN0
#define PROBE_TASK_1					PIN1
#define PROBE_TASK_2					PIN2	
#define PROBE_TASK_3					PIN3
#define PROBE_TASK_4					PIN4	
#define PROBE_TASK_5					PIN5
#define PROBE_TASK_6					PIN6				
#define PROBE_IDLE						PIN7
														
#define PERIODICITY_TASK_1		((uint8_t)50)													
#define PERIODICITY_TASK_2		((uint8_t)50)													
#define PERIODICITY_TASK_3		((uint8_t)100)													
#define PERIODICITY_TASK_4		((uint8_t)20)																																						
#define PERIODICITY_TASK_5		((uint8_t)10)
#define PERIODICITY_TASK_6		((uint8_t)100)
														
#define ET_TASK_5						  ((uint8_t)5)
#define ET_TASK_6							((uint8_t)12)														
														
#define ET_2_COUNT_MAP				((uint16_t)6666)
#define DUMMY_ET(ET)																									\
					do{																													\
							uint32_t  i;																						\
							for(i=0; i<(ET * ET_2_COUNT_MAP); i++){									\
								i=i;																									\
							}																												\
					}while(0)

					
					
#define MAX_QUEUE_WAIT_TIME 		((uint8_t)5)
#define QUEUE_LENGTH						((uint8_t)10)	
#define MESSAGE_BUFFER_SIZE			((uint8_t)25)

#define PULSE_TICK() 																										\
					do{																														\
						GPIO_write(PROBE_PORT, PROBE_TICK, PIN_IS_HIGH);						\
						GPIO_write(PROBE_PORT, PROBE_TICK, PIN_IS_LOW);							\
					}while(0)	

/* Constants to setup I/O and processor. */
#define mainBUS_CLK_FULL	( ( unsigned char ) 0x01 )

/* Constants for the ComTest demo application tasks. */
#define mainCOM_TEST_BAUD_RATE	( ( unsigned long ) 115200 )


typedef struct{
	uint32_t inTime;
	uint32_t outTime;
	uint32_t totalTime;
}TaskTime_t;

typedef enum{
	RISING,
	FALLING,
	NO_CHANGE
}buttonEdge_t;

typedef struct{
		uint8_t ucMessageID;
		uint8_t ucData[MESSAGE_BUFFER_SIZE];
}message_t;



QueueHandle_t xQueueConsumer;

struct{
	TaskTime_t taskTime[NUMBER_OF_TASKS];
	uint32_t systemTime;
	uint32_t cpu_load;
}performanceEvaluation;

void Task_1(void *param);
void Task_2(void *param);
void Task_3(void *param);
void Task_4(void *param);
void Task_5(void *param);
void Task_6(void *param);

void vApplicationTickHook(void);
void Intialize_TaskTime(TaskTime_t * inputTaskTime);
static void prvSetupHardware( void );

#if ( configUSE_EDF_SCHEDULER == 1 )
	BaseType_t xTaskPeriodicCreate( TaskFunction_t pxTaskCode,
													const char * const pcName, /*lint !e971 Unqualified char types are allowed for strings and single characters only. */
													const configSTACK_DEPTH_TYPE usStackDepth,
													void * const pvParameters,
													UBaseType_t uxPriority,
													TaskHandle_t * const pxCreatedTask ,
													TickType_t period);
#endif

int main( void )
{
	/* Setup the hardware for use with the Keil demo board. */
	prvSetupHardware();
	Intialize_TaskTime(performanceEvaluation.taskTime);
	
	xQueueConsumer = xQueueCreate( QUEUE_LENGTH, sizeof( message_t ) );

	xTaskPeriodicCreate(
	Task_1, 
	"Button_1_Monitor", 
	configMINIMAL_STACK_SIZE, 
	(void *)NULL,
	0, 
	NULL, 
	PERIODICITY_TASK_1);
	
xTaskPeriodicCreate(
	Task_2, 
	"Button_2_Monitor", 
	configMINIMAL_STACK_SIZE, 
	(void *)NULL,
	0, 
	NULL, 
	PERIODICITY_TASK_2);
	
xTaskPeriodicCreate(
	Task_3, 
	"Periodic_Transmitter", 
	configMINIMAL_STACK_SIZE, 
	(void *)NULL,
	0, 
	NULL, 
	PERIODICITY_TASK_3);

	
	xTaskPeriodicCreate(
	Task_4, 
	"Uart_Receiver", 
	configMINIMAL_STACK_SIZE, 
	(void *)NULL,
	0, 
	NULL, 
	PERIODICITY_TASK_4);
	
xTaskPeriodicCreate(
	Task_5, 
	"Load_1_Simulation", 
	configMINIMAL_STACK_SIZE, 
	(void *)NULL,
	0, 
	NULL, 
	PERIODICITY_TASK_5);
	
xTaskPeriodicCreate(
	Task_6, 
	"Load_2_Simulation", 
	configMINIMAL_STACK_SIZE, 
	(void *)NULL, 
	0, 
	NULL, 
	PERIODICITY_TASK_6);
	
	
	/* Now all the tasks have been started - start the scheduler.

	NOTE : Tasks run in system mode and the scheduler runs in Supervisor mode.
	The processor MUST be in supervisor mode when vTaskStartScheduler is 
	called.  The demo applications included in the FreeRTOS.org download switch
	to supervisor mode prior to main being called.  If you are not using one of
	these demo application projects then ensure Supervisor mode is used here. */
	vTaskStartScheduler();

	/* Should never reach here!  If you do then there was not enough heap
	available for the idle task to be created. */
	for( ;; );
}
/*-----------------------------------------------------------*/

/* Function to reset timer 1 */
void timer1Reset(void)
{
	T1TCR |= 0x2;
	T1TCR &= ~0x2;
}

/* Function to initialize and start timer 1 */
static void configTimer1(void)
{
	T1PR = 1000;
	T1TCR |= 0x1;
}

static void prvSetupHardware( void )
{
	/* Perform the hardware setup required.  This is minimal as most of the
	setup is managed by the settings in the project file. */

	/* Configure UART */
	xSerialPortInitMinimal(mainCOM_TEST_BAUD_RATE);

	/* Configure GPIO */
	GPIO_init();
	
	/* Config trace timer 1 and read T1TC to get current tick */
	configTimer1();

	/* Setup the peripheral bus to be the same as the PLL output. */
	VPBDIV = mainBUS_CLK_FULL;
}
/*-----------------------------------------------------------*/


void Intialize_TaskTime(TaskTime_t * inputTaskTime){
	uint8_t i;
	for(i = 0; i<NUMBER_OF_TASKS; i++){
		(inputTaskTime + i)->inTime = 0;
		(inputTaskTime + i)->outTime = 0;
		(inputTaskTime + i)->totalTime = 0;
	}
}
					
void vApplicationTickHook(void){
		PULSE_TICK();
}


/********************************************** Task 1 **********************/
void Task_1(void *param){
	TickType_t xLastWakeTime;
	message_t xMessegeToSend = {0, 0};
	bool sendMessege = false;
	
	pinState_t bCurrentButtonState, bPriviousButtonState = GPIO_read(PORT_1, PIN0);
	buttonEdge_t buttonEdge;
	
	vTaskSetApplicationTaskTag(NULL, (TaskHookFunction_t)PROBE_TASK_1);
	
	xLastWakeTime = xTaskGetTickCount();
	for(;;){
		
		//NOTE: This is bad practice and it definitely should be performed whitin the ISR with the EDGE detection hardware not in a task
		bCurrentButtonState = GPIO_read(PORT_1, PIN0);
		if(bCurrentButtonState != bPriviousButtonState){
			//start Critical Section
			bPriviousButtonState = bCurrentButtonState;
			if(bCurrentButtonState == PIN_IS_HIGH){
				 buttonEdge = RISING;
			}else{
				 buttonEdge = FALLING;
			}
			//End Critical Section
		}else{
			buttonEdge = NO_CHANGE;
		}
		
		switch((int)buttonEdge){
			case RISING:{
					strcpy((char *)(xMessegeToSend.ucData), "Btn1 Rising detected");		
					xMessegeToSend.ucData[MESSAGE_BUFFER_SIZE-1] = '\n';
					sendMessege = true;
				break;
			}
			case FALLING:{
					strcpy((char *)(xMessegeToSend.ucData), "Btn1 Falling detected");		
					xMessegeToSend.ucData[MESSAGE_BUFFER_SIZE-1] = '\n';
					sendMessege = true;
				break;
			}
			default: {
					sendMessege = false;
				break;
			}
		}
		
		if(sendMessege == true){
			xQueueSendToBack(xQueueConsumer, &xMessegeToSend, MAX_QUEUE_WAIT_TIME);
		}
		vTaskDelayUntil( &xLastWakeTime, PERIODICITY_TASK_1 );
	}
}

///********************************************** Task 2 **********************/
void Task_2(void *param){
	TickType_t xLastWakeTime;
	message_t xMessegeToSend = {0, 0};
	bool sendMessege = false;
	
	pinState_t bCurrentButtonState, bPriviousButtonState = GPIO_read(PORT_1, PIN1);
	buttonEdge_t buttonEdge;
	
	vTaskSetApplicationTaskTag(NULL, (TaskHookFunction_t)PROBE_TASK_2);
	
	xLastWakeTime = xTaskGetTickCount();
	for(;;){
		
		//NOTE: This is bad practice and it definitely should be performed whitin the ISR with the EDGE detection hardware not in a task
		bCurrentButtonState = GPIO_read(PORT_1, PIN1);
		if(bCurrentButtonState != bPriviousButtonState){
			//start Critical Section
			bPriviousButtonState = bCurrentButtonState;
			if(bCurrentButtonState == PIN_IS_HIGH){
				 buttonEdge = RISING;
			}else{
				 buttonEdge = FALLING;
			}
			//End Critical Section
		}else{
			buttonEdge = NO_CHANGE;
		}
		
		switch((int)buttonEdge){
			case RISING:{
					strcpy((char *)(xMessegeToSend.ucData), "Btn2 Rising detected");		
					xMessegeToSend.ucData[MESSAGE_BUFFER_SIZE-1] = '\n';
					sendMessege = true;
				break;
			}
			case FALLING:{
					strcpy((char *)(xMessegeToSend.ucData), "Btn2 Falling detected");		
					xMessegeToSend.ucData[MESSAGE_BUFFER_SIZE-1] = '\n';
					sendMessege = true;
				break;
			}
			default: {
					sendMessege = false;
				break;
			}
		}
		
		if(sendMessege == true){
			xQueueSendToBack(xQueueConsumer, &xMessegeToSend, MAX_QUEUE_WAIT_TIME);
		}
		vTaskDelayUntil( &xLastWakeTime, PERIODICITY_TASK_2);
	}
}

/********************************************** Task 3 **********************/
void Task_3(void *param){
	TickType_t xLastWakeTime;
	message_t xMessegeToSend = {0, 0};
	xMessegeToSend.ucMessageID = '3';

	vTaskSetApplicationTaskTag(NULL, (TaskHookFunction_t)PROBE_TASK_3);
	
	xLastWakeTime = xTaskGetTickCount();
	for(;;){
		strcpy((char *)(xMessegeToSend.ucData), "Future Work is Digital!");		//TODO: add rand()
		xMessegeToSend.ucData[MESSAGE_BUFFER_SIZE-1] = '\n';
		
		xQueueSendToBack(xQueueConsumer, &xMessegeToSend, MAX_QUEUE_WAIT_TIME);
		vTaskDelayUntil( &xLastWakeTime, PERIODICITY_TASK_3 );
	}
}

 

/********************************************** Task 4 **********************/
void Task_4(void *param){
	TickType_t xLastWakeTime;
	message_t xMessageBuffer;
	
	
	vTaskSetApplicationTaskTag(NULL, (TaskHookFunction_t)PROBE_TASK_4);
	
	xLastWakeTime = xTaskGetTickCount();
	for(;;){

			if( xQueueReceive( xQueueConsumer,
                         &( xMessageBuffer ),
                         ( TickType_t ) MAX_QUEUE_WAIT_TIME) == pdPASS )
      {
         vSerialPutString((signed char *)(xMessageBuffer.ucData), MESSAGE_BUFFER_SIZE);
      }
			
		vTaskDelayUntil( &xLastWakeTime, PERIODICITY_TASK_4);
	}
}


/********************************************** Task 5 **********************/
void Task_5(void *param){
	TickType_t xLastWakeTime;
	vTaskSetApplicationTaskTag(NULL, (TaskHookFunction_t)PROBE_TASK_5);
	
	xLastWakeTime = xTaskGetTickCount();
	for(;;){
		DUMMY_ET(ET_TASK_5);
		vTaskDelayUntil( &xLastWakeTime, PERIODICITY_TASK_5 );
	}
}


/********************************************** Task 6 **********************/
void Task_6(void *param){
	TickType_t xLastWakeTime;
	vTaskSetApplicationTaskTag(NULL, (TaskHookFunction_t)PROBE_TASK_6);
	
	xLastWakeTime = xTaskGetTickCount();
	for(;;){	
		DUMMY_ET(ET_TASK_6);
		vTaskDelayUntil( &xLastWakeTime, PERIODICITY_TASK_6 );
	}
}



