

#ifndef GPIO_H_
#define GPIO_H_

/************* Type def section ************/

/* Port data type */
typedef enum
{
	PORT_0,
	PORT_1,

}portX_t;

/* Pin data type */
typedef enum
{
	PIN0 = 16,
	PIN1,
	PIN2,
	PIN3,
	PIN4,
	PIN5,
	PIN6,
	PIN7,
	PIN8,
	PIN9,
	PIN10,
	PIN11,
	PIN12,
	PIN13,
	PIN14,
	PIN15,
	
}pinX_t;

typedef enum
{
	INPUT,
	OUTPUT
	
}pinDir_t;

typedef enum
{
	PIN_IS_LOW,
	PIN_IS_HIGH

}pinState_t;


/************ Function declaration section ***********/

extern void GPIO_init(void);
extern pinState_t GPIO_read(portX_t PortName, pinX_t pinNum);
extern void GPIO_write(portX_t PortName, pinX_t PinNum, pinState_t pinState);



#endif /* DIO_MCAL_INC_DIO_H_ */
