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
	BASIC INTERRUPT DRIVEN SERIAL PORT DRIVER FOR UART0. 

	Note this driver is used to test the FreeRTOS port.  It is NOT intended to
	be an example of an efficient implementation!
*/

/* Standard includes. */
#include <stdlib.h>

/* Scheduler includes. */
#include "FreeRTOS.h"
#include "queue.h"
#include "task.h"

/* Demo application includes. */
#include "serial.h"

/*-----------------------------------------------------------*/

/* Constants to setup I/O */
#define mainTX_ENABLE		( ( unsigned long ) 0x00010000 )	/* UART1. */
#define mainRX_ENABLE		( ( unsigned long ) 0x00040000 ) 	/* UART1. */

/* Constants to setup and access the UART. */
#define serDLAB							( ( unsigned char ) 0x80 )
#define serENABLE_INTERRUPTS			( ( unsigned char ) 0x03 )
#define serNO_PARITY					( ( unsigned char ) 0x00 )
#define ser1_STOP_BIT					( ( unsigned char ) 0x00 )
#define ser8_BIT_CHARS					( ( unsigned char ) 0x03 )
#define serFIFO_ON						( ( unsigned char ) 0x01 )
#define serCLEAR_FIFO					( ( unsigned char ) 0x06 )
#define serWANTED_CLOCK_SCALING			( ( unsigned long ) 16 )

/* Constants to setup and access the VIC. */
#define serU1VIC_CHANNEL				( ( unsigned long ) 0x0007 )
#define serU1VIC_CHANNEL_BIT			( ( unsigned long ) 0x0080 )
#define serU1VIC_ENABLE					( ( unsigned long ) 0x0020 )

/* Constant to access the VIC. */
#define serCLEAR_VIC_INTERRUPT			( ( unsigned long ) 0 )

/* Constants to determine the ISR source. */
#define serSOURCE_THRE					( ( unsigned char ) 0x02 )
#define serSOURCE_RX_TIMEOUT			( ( unsigned char ) 0x0c )
#define serSOURCE_ERROR					( ( unsigned char ) 0x06 )
#define serSOURCE_RX					( ( unsigned char ) 0x04 )
#define serINTERRUPT_SOURCE_MASK		( ( unsigned char ) 0x0f )
#define serINTERRUPT_IS_PENDING			( ( unsigned char ) 0x01 )

/*-----------------------------------------------------------*/
unsigned char receivedChar;
unsigned char isNewCharAvailable = 0;

unsigned char txBuffer[200];
unsigned char txDataSizeToSend;
unsigned char txDataSizeLeftToSend;
/*
 * The asm wrapper for the interrupt service routine.
 */
extern void vUART_ISREntry( void );

/* 
 * The C function called from the asm wrapper. 
 */
void vUART_ISRHandler( void );

/*-----------------------------------------------------------*/

void xSerialPortInitMinimal( unsigned long ulWantedBaud)
{
    unsigned long ulDivisor, ulWantedClock;
	
	/* Configure the UART1 pins.  All other pins remain at their default of 0. */
	PINSEL0 |= mainTX_ENABLE;
	PINSEL0 |= mainRX_ENABLE;

	/* Setup the baud rate:  Calculate the divisor value. */
	ulWantedClock = ulWantedBaud * serWANTED_CLOCK_SCALING;
	ulDivisor = configCPU_CLOCK_HZ / ulWantedClock;

	/* Set the DLAB bit so we can access the divisor. */
	U1LCR |= serDLAB;

	/* Setup the divisor. */
	U1DLL = ( unsigned char ) ( ulDivisor & ( unsigned long ) 0xff );
	ulDivisor >>= 8;
	U1DLM = ( unsigned char ) ( ulDivisor & ( unsigned long ) 0xff );

	/* Turn on the FIFO's and clear the buffers. */
	U1FCR = ( serFIFO_ON | serCLEAR_FIFO );

	/* Setup transmission format. */
	U1LCR = serNO_PARITY | ser1_STOP_BIT | ser8_BIT_CHARS;

	/* Setup the VIC for the UART. */
	VICIntSelect &= ~( serU1VIC_CHANNEL_BIT );
	VICIntEnable |= serU1VIC_CHANNEL_BIT;
	VICVectAddr1 = ( unsigned long ) vUART_ISREntry;
	VICVectCntl1 = serU1VIC_CHANNEL | serU1VIC_ENABLE;

	/* Enable UART0 interrupts. */
	U1IER |= serENABLE_INTERRUPTS;
}
/*-----------------------------------------------------------*/

signed portBASE_TYPE xSerialGetChar(signed char *pcRxedChar)
{
	/* Get the next character from the buffer.  Return false if no characters
	are available, or arrive before xBlockTime expires. */
	if(isNewCharAvailable == 1U)
	{
		isNewCharAvailable = 0U;
		*pcRxedChar = receivedChar;
		return pdTRUE;
	}
	else
	{
		return pdFALSE;
	}
}
/*-----------------------------------------------------------*/

signed portBASE_TYPE vSerialPutString(const signed char * const pcString, unsigned short usStringLength )
{
	int i;

	if(txDataSizeLeftToSend == 0 && pcString != NULL)
	{
	  txDataSizeToSend = usStringLength;
	  txDataSizeLeftToSend = usStringLength;
	
	  for(i = 0;i < usStringLength; i++)
	  {
		  txBuffer[i] = pcString[i];
	  }
	
	  xSerialPutChar(txBuffer[txDataSizeToSend - txDataSizeLeftToSend--]);
		
	  return pdTRUE;
	}
	else
	{
		return pdFALSE;
	}
}
/*-----------------------------------------------------------*/

void xSerialPutChar(signed char cOutChar)
{
	U1THR = cOutChar;
}
/*-----------------------------------------------------------*/

void vUART_ISRHandler( void )
{
signed char cChar;
unsigned char ucInterrupt;

	ucInterrupt = U1IIR;

	/* The interrupt pending bit is active low. */
	while( ( ucInterrupt & serINTERRUPT_IS_PENDING ) == 0 )
	{
		/* What caused the interrupt? */
		switch( ucInterrupt & serINTERRUPT_SOURCE_MASK )
		{
			case serSOURCE_ERROR :	/* Not handling this, but clear the interrupt. */
				cChar = U1LSR;
				break;
	
			case serSOURCE_THRE	:	/* The THRE is empty */
				
				/* Do nothing */
				if(txDataSizeLeftToSend > 0)
				{
					U1THR = txBuffer[txDataSizeToSend - txDataSizeLeftToSend--];
				}
				
				break;
	
			case serSOURCE_RX_TIMEOUT :
			case serSOURCE_RX	:	/* A character was received */
			
				receivedChar = U1RBR;
				isNewCharAvailable = 1U;			
				break;
	
			default:	/* There is nothing to do, leave the ISR. */
				break;
		}

		ucInterrupt = U1IIR;
	}

	/* Clear the ISR in the VIC. */
	VICVectAddr = serCLEAR_VIC_INTERRUPT;
}
/*-----------------------------------------------------------*/






	
