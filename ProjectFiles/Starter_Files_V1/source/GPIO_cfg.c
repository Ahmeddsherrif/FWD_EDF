#include <stdint.h>
#include "GPIO.h"
#include "GPIO_cfg.h"


PinConfig_t PinConfig_array[] = 
							{
								{PORT_0, PIN0, OUTPUT},
								{PORT_0, PIN1, OUTPUT},
								{PORT_0, PIN2, OUTPUT},
								{PORT_0, PIN3, OUTPUT},
								{PORT_0, PIN4, OUTPUT},
								{PORT_0, PIN5, OUTPUT},
								{PORT_0, PIN6, OUTPUT},
								{PORT_0, PIN7, OUTPUT},
								{PORT_0, PIN8, OUTPUT},
								{PORT_0, PIN9, OUTPUT},
								{PORT_0, PIN10, OUTPUT},
								{PORT_0, PIN11, OUTPUT},
								{PORT_0, PIN13, OUTPUT},
								{PORT_0, PIN14, OUTPUT},
								{PORT_0, PIN15, OUTPUT},
								
								{PORT_1, PIN0, INPUT},
								{PORT_1, PIN1, INPUT},
								{PORT_1, PIN2, INPUT},
								{PORT_1, PIN3, INPUT},
								{PORT_1, PIN4, OUTPUT},
								{PORT_1, PIN5, OUTPUT},
								{PORT_1, PIN6, OUTPUT},
								{PORT_1, PIN7, OUTPUT},
								{PORT_1, PIN8, OUTPUT},
								{PORT_1, PIN9, OUTPUT},
								{PORT_1, PIN10, OUTPUT},
								{PORT_1, PIN11, OUTPUT},
								{PORT_1, PIN13, OUTPUT},
								{PORT_1, PIN14, OUTPUT},
								{PORT_1, PIN15, OUTPUT},
							};

uint16_t PinConfig_array_size = sizeof(PinConfig_array)/sizeof(PinConfig_t);
