#ifndef _MAIN_H_
#define _MAIN_H_

#include <stdint.h>

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
					
#define TO_ASCII(num)						(num + (uint8_t)48)

/* Constants to setup I/O and processor. */
#define mainBUS_CLK_FULL	( ( unsigned char ) 0x01 )

/* Constants for the ComTest demo application tasks. */
#define mainCOM_TEST_BAUD_RATE	( ( unsigned long ) 115200 )


typedef enum{
	RISING,
	FALLING,
	NO_CHANGE
}buttonEdge_t;

typedef struct{
		uint8_t ucMessageID;
		char ucData[MESSAGE_BUFFER_SIZE];
}message_t;

typedef struct{
	uint32_t inTime;
	//uint32_t outTime;
	uint32_t totalTime;
}TaskTime_t;

typedef struct{
	TaskTime_t taskTime[NUMBER_OF_TASKS + 1];
	uint32_t temp;
	uint32_t cpu_Load;
}performanceEvaluation_t;





void Task_1(void *param);
void Task_2(void *param);
void Task_3(void *param);
void Task_4(void *param);
void Task_5(void *param);
void Task_6(void *param);

void vApplicationTickHook(void);
void vApplicationIdleHook(void);

static void prvSetupHardware( void );

void vInt2String (uint32_t inInteger, char * outString);
void vStringClear(char * inOutString);

#if ( configUSE_EDF_SCHEDULER == 1 )
	BaseType_t xTaskPeriodicCreate( TaskFunction_t pxTaskCode,
													const char * const pcName, /*lint !e971 Unqualified char types are allowed for strings and single characters only. */
													const configSTACK_DEPTH_TYPE usStackDepth,
													void * const pvParameters,
													UBaseType_t uxPriority,
													TaskHandle_t * const pxCreatedTask ,
													TickType_t period);
#endif


#endif /*_MAIN_H_*/