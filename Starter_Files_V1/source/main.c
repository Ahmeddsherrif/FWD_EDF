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

/* Scheduler includes. */
#include "FreeRTOS.h"
#include "task.h"
#include "lpc21xx.h"

/* Peripheral includes. */
#include "serial.h"
#include "GPIO.h"


/*-----------------------------------------------------------*/

/* Constants to setup I/O and processor. */
#define mainBUS_CLK_FULL	( ( unsigned char ) 0x01 )

/* Constants for the ComTest demo application tasks. */
#define mainCOM_TEST_BAUD_RATE	( ( unsigned long ) 115200 )


/*
 * Configure the processor for use with the Keil demo board.  This is very
 * minimal as most of the setup is managed by the settings in the project
 * file.
 */
static void prvSetupHardware( void );
/*-----------------------------------------------------------*/

#if ( configUSE_EDF_SCHEDULER == 1 )

    BaseType_t xTaskPeriodicCreate( TaskFunction_t pxTaskCode,
                            const char * const pcName, /*lint !e971 Unqualified char types are allowed for strings and single characters only. */
                            const configSTACK_DEPTH_TYPE usStackDepth,
                            void * const pvParameters,
                            UBaseType_t uxPriority,
                            TaskHandle_t * const pxCreatedTask ,
                            TickType_t period);
#endif

#define PERIODICITY_TASK_1		10
#define ET_TASK_1						  2
														
#define PERIODICITY_TASK_2		10
#define ET_TASK_2							2
														
#define PERIODICITY_TASK_3		10
#define ET_TASK_3						  1
														
#define PERIODICITY_TASK_4		10
#define ET_TASK_4							1
																																								
#define PERIODICITY_TASK_5		10
#define ET_TASK_5						  1
														
#define PERIODICITY_TASK_6		10
#define ET_TASK_6							1
														
																										

														
#define ET_2_COUNT_MAP		6666
#define DUMMY_ET(ET)																									\
					do{																													\
							int  i;																									\
							for(i=0; i<(ET * ET_2_COUNT_MAP); i++){									\
								i=i;																									\
							}																												\
					}while(0)
					
//trace macros if ndef
#define PROBE_PORT		  	PORT_0	
					
#define PROBE_TICK				PIN0
#define PROBE_TASK_1			PIN1
#define PROBE_TASK_2			PIN2	
#define PROBE_TASK_3			PIN3
#define PROBE_TASK_4			PIN4	
#define PROBE_TASK_5			PIN5
#define PROBE_TASK_6			PIN6				
#define PROBE_IDLE				PIN7
					

					
					
#define PULSE_TASK_IN(probe)			GPIO_write(PROBE_PORT,probe, PIN_IS_HIGH)
#define PULSE_TASK_OUT(probe)			GPIO_write(PROBE_PORT,probe, PIN_IS_LOW)
		
					
#define PULSE_IDLE_IN()						GPIO_write(PROBE_PORT,PROBE_IDLE, PIN_IS_HIGH)
#define PULSE_IDLE_OUT()					GPIO_write(PROBE_PORT,PROBE_IDLE, PIN_IS_LOW)

#define PULSE_TICK() 																									\
					do{																													\
						GPIO_write(PROBE_PORT,PROBE_TICK, PIN_IS_HIGH);						\
						GPIO_write(PROBE_PORT,PROBE_TICK, PIN_IS_LOW);						\
					}while(0)	

					
void vApplicationTickHook(){
		PULSE_TICK();
}

void vApplicationIdleHook(){
		PULSE_IDLE_IN();
}


/********************************************** Task 1 **********************/
void Task_1(void *param){
	TickType_t xLastWakeTime;
	xLastWakeTime = xTaskGetTickCount();
	
	for(;;){
		PULSE_IDLE_OUT();
		
		PULSE_TASK_IN(PROBE_TASK_1);	
		DUMMY_ET(ET_TASK_1);
		PULSE_TASK_OUT(PROBE_TASK_1);
		
		vTaskDelayUntil( &xLastWakeTime, PERIODICITY_TASK_1 );
	}
}

/********************************************** Task 2 **********************/
void Task_2(void *param){
	TickType_t xLastWakeTime;
	xLastWakeTime = xTaskGetTickCount();
	
	for(;;){
		PULSE_IDLE_OUT();
		
		PULSE_TASK_IN(PROBE_TASK_2);	
		DUMMY_ET(ET_TASK_2);
		PULSE_TASK_OUT(PROBE_TASK_2);
		
		vTaskDelayUntil( &xLastWakeTime, PERIODICITY_TASK_2 );
	}
}

/********************************************** Task 3 **********************/
void Task_3(void *param){
	TickType_t xLastWakeTime;
	xLastWakeTime = xTaskGetTickCount();
	
	for(;;){
		PULSE_IDLE_OUT();
		
		PULSE_TASK_IN(PROBE_TASK_3);	
		DUMMY_ET(ET_TASK_3);
		PULSE_TASK_OUT(PROBE_TASK_3);
		
		vTaskDelayUntil( &xLastWakeTime, PERIODICITY_TASK_3 );
	}
}

/********************************************** Task 4 **********************/
void Task_4(void *param){
	TickType_t xLastWakeTime;
	xLastWakeTime = xTaskGetTickCount();
	
	for(;;){
		PULSE_IDLE_OUT();
		
		PULSE_TASK_IN(PROBE_TASK_4);	
		DUMMY_ET(ET_TASK_4);
		PULSE_TASK_OUT(PROBE_TASK_4);
		
		vTaskDelayUntil( &xLastWakeTime, PERIODICITY_TASK_4 );
	}
}


/********************************************** Task 5 **********************/
void Task_5(void *param){
	TickType_t xLastWakeTime;
	xLastWakeTime = xTaskGetTickCount();
	
	for(;;){
		PULSE_IDLE_OUT();
		
		PULSE_TASK_IN(PROBE_TASK_5);	
		DUMMY_ET(ET_TASK_5);
		PULSE_TASK_OUT(PROBE_TASK_5);
		
		vTaskDelayUntil( &xLastWakeTime, PERIODICITY_TASK_5 );
	}
}


/********************************************** Task 6 **********************/
void Task_6(void *param){
	TickType_t xLastWakeTime;
	xLastWakeTime = xTaskGetTickCount();
	
	for(;;){
		PULSE_IDLE_OUT();
		
		PULSE_TASK_IN(PROBE_TASK_6);	
		DUMMY_ET(ET_TASK_6);
		PULSE_TASK_OUT(PROBE_TASK_6);
		
		vTaskDelayUntil( &xLastWakeTime, PERIODICITY_TASK_6 );
	}
}


/*
 * Application entry point:
 * Starts all the other tasks, then starts the scheduler. 
 */
int main( void )
{
	/* Setup the hardware for use with the Keil demo board. */
	prvSetupHardware();


xTaskPeriodicCreate(
	Task_1, 
	"Button_1_Monitor", 
	100, 
	(void *)NULL,
	0, 
	NULL, 
	PERIODICITY_TASK_1);
	
xTaskPeriodicCreate(
	Task_2, 
	"Button_2_Monitor", 
	100, 
	(void *)NULL,
	0, 
	NULL, 
	PERIODICITY_TASK_2);
	
xTaskPeriodicCreate(
	Task_3, 
	"Periodic_Transmitter", 
	100, 
	(void *)NULL,
	0, 
	NULL, 
	PERIODICITY_TASK_3);

	
	xTaskPeriodicCreate(
	Task_4, 
	"Uart_Receiver", 
	100, 
	(void *)NULL,
	0, 
	NULL, 
	PERIODICITY_TASK_4);
	
xTaskPeriodicCreate(
	Task_5, 
	"Load_1_Simulation", 
	100, 
	(void *)NULL,
	0, 
	NULL, 
	PERIODICITY_TASK_5);
	
xTaskPeriodicCreate(
	Task_6, 
	"Load_2_Simulation", 
	100, 
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


