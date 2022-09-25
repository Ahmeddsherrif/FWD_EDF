

#include <stdint.h>
#include "bit_math.h"
#include "main.h"
#include "GPIO.h"
#include "GPIO_cfg.h"
#include "lpc21xx.h"


static const pinX_t tagToPinMap[NUMBER_OF_TASKS + 1] = {
		PROBE_IDLE,							//idleTask
		PROBE_TASK_1,						//Task1
		PROBE_TASK_2,						//Task2
		PROBE_TASK_3,						//Task3
		PROBE_TASK_4,						//Task4
		PROBE_TASK_5,						//Task5
		PROBE_TASK_6						//Task6
};

void GPIO_init(void)
{
	int i;
	
	for(i = 0; i < PinConfig_array_size;i++)
	{
		switch(PinConfig_array[i].Port)
		{
			case PORT_0:
				if(INPUT == PinConfig_array[i].Direction)
				{
					CLR_BIT(IODIR0, PinConfig_array[i].Pin);
				}
				else if (OUTPUT == PinConfig_array[i].Direction)
				{
					SET_BIT(IODIR0, PinConfig_array[i].Pin);
				}
				else
				{
					
				}
				break;
	
			case PORT_1:
				if(INPUT == PinConfig_array[i].Direction)
				{
					CLR_BIT(IODIR1, PinConfig_array[i].Pin);
				}
				else if (OUTPUT == PinConfig_array[i].Direction)
				{
					SET_BIT(IODIR1, PinConfig_array[i].Pin);
				}
				else
				{

				}
				break;
	
			default:
		}
	}
}


pinState_t GPIO_read(portX_t PortName, pinX_t pinNum)
{
	pinState_t state;
	
	switch(PortName)
	{
		case PORT_0:
			{
				state = (pinState_t) GET_BIT(IOPIN0, pinNum);
				break;
			}

		case PORT_1:
			{
				state = (pinState_t) GET_BIT(IOPIN1, pinNum);
				break;
			}
	}
	
	return state;
}


void GPIO_write(portX_t portName, pinX_t pinNum, pinState_t pinState)
{
	switch(portName)
	{
		case PORT_0:
			if(PIN_IS_LOW == pinState)
			{
				SET_BIT(IOCLR0, pinNum);
			}
			else if (PIN_IS_HIGH == pinState)
			{
				SET_BIT(IOSET0, pinNum);
			}
			else
			{
				
			}
			break;

		case PORT_1:
			if(PIN_IS_LOW == pinState)
			{
				SET_BIT(IOCLR1, pinNum);
			}
			else if (PIN_IS_HIGH == pinState)
			{
				SET_BIT(IOSET1, pinNum);
			}
			else
			{
				
			}
	}
}

pinX_t TagToPinMap (uint8_t taskTag){
	return tagToPinMap[taskTag];
}
