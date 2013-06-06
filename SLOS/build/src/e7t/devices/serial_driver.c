/*
 *  ____________________________________________________________________
 * 
 *  Copyright (c) 2002, Andrew N. Sloss, Chris Wright and Dominic Symes
 *  All rights reserved.
 *  ____________________________________________________________________
 * 
 *  NON-COMMERCIAL USE License
 *  
 *  Redistribution and use in source and binary forms, with or without 
 *  modification, are permitted provided that the following conditions 
 *  are met: 
 *  
 *  1. For NON-COMMERCIAL USE only.
 * 
 *  2. Redistributions of source code must retain the above copyright 
 *     notice, this list of conditions and the following disclaimer. 
 * 
 *  3. Redistributions in binary form must reproduce the above 
 *     copyright notice, this list of conditions and the following 
 *     disclaimer in the documentation and/or other materials provided 
 *     with the distribution. 
 * 
 *  4. All advertising materials mentioning features or use of this 
 *     software must display the following acknowledgement:
 * 
 *     This product includes software developed by Andrew N. Sloss,
 *     Chris Wright and Dominic Symes. 
 * 
 *   THIS SOFTWARE IS PROVIDED BY THE CONTRIBUTORS ``AS IS'' AND ANY 
 *   EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE 
 *   IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR 
 *   PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL THE CONTRIBUTORS BE 
 *   LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, 
 *   OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, 
 *   PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, 
 *   OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY 
 *   THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR 
 *   TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT 
 *   OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY 
 *   OF SUCH DAMAGE. 
 * 
 *  If you have questions about this license or would like a different
 *  license please email :
 * 
 * 	andrew@sloss.net
 * 
 * 
 */
 
/*****************************************************************************
 * Simple Little Operating System - SLOS
 *****************************************************************************/

/*****************************************************************************
 *
 * Module      : serial_driver.c
 * Description : low level device driver for the serial device
 *               based on DDF v0.01. 
 * OS          : SLOS 0.09
 * Platform    : e7t
 * History     :
 *
 * 24th November 2001 Andrew N. Sloss
 * - create template to speed up driver development.
 *
 *****************************************************************************/

/*****************************************************************************
 * IMPORT
 *****************************************************************************/

#include "../../devices/ddf_frame.h"
#include "serial_driver.h"

#define DEVICE_SERIAL_E7T  55090

typedef	unsigned long		u32;

/*****************************************************************************
 * MACROS
 *****************************************************************************/
#define	__REG	*(volatile unsigned long *)
#define	__REG8	*(volatile unsigned char *)
#define	__REG32	*(volatile unsigned long *)

#define	TICKS_PER_SECOND 32768
#define	RCNR	 __REG(0x40900000)

/*
 * Serial settings.......................
 */

// Closk registers
#define	CLOCK_MANAGER_BASE 0x41300000
#define	CKEN		0x04

//GPIO registers
#define	GPIO_BASE	0x40E00000
#define	GPDR1		0x10
#define	GAFR1_L	0x5C

/*
 * Line control register bits............
 */

/*
 * UART Control Register bits............
 */

#define	FFUART_BASE       0x40100000
#define	FFRBR		0x00
#define	FFTHR		0x00
#define	FFIER		0x04
#define	FFFCR		0x08
#define	FFLCR		0x0C
#define	FFLSR		0x14
#define	FFDLL		0x00
#define	FFDLH		0x04


/* default baud rate value */
 
#define BAUD_9600	   (162 << 4)

/* UART registers are on word aligned, D8 */

/* UART primitives */

#define COM1	(1)
#define COM0	(0)

/*****************************************************************************
 * DATATYPES
 *****************************************************************************/

typedef struct 
{
unsigned int baudrate[2]; /* baud rate of the serial port ............ */
unsigned int uid[2];      /* lock[0] = COM1 lock[1] = COM2 ........... */
} internal_serialstr;

/*****************************************************************************
 * STATICS
 *****************************************************************************/

internal_serialstr		node;

/* -- internalSerialE7TSetup --------------------------------------------------
 *
 * Description : internal serial port setup routine
 *
 * Parameters  : unsigned int port - UART address
 *             : unsigned int baud - baud rate
 * Return      : none...
 * Notes       :
 * 
 */

void internalSerialE7TSetup(unsigned int port, unsigned int baud){}

/* -- serial_init -------------------------------------------------------------
 *
 * Description : initalize serial driver.
 * 
 * Parameters  : none...
 * Return      : none...
 * Notes       : none...
 *
 *  Initializes the node data structures
 */

void serial_init(void)
{
/* initalize physical device ... */

/* initialize ......................... */

	/*         GPIO SETTING            */
	/*
	Set GPIO's Pin Direction and Alternate Function Register
	GPIO 34, GPIO39 
	GPDR1 = 0 : input, 1 : output
	GAFR1_L = AF34 : set 01 to FFUART receive, AF39 : set 10 to FFUART transmit data
	*/
	__REG32(GPIO_BASE + GPDR1) = 0x00000080; // set PD39 to 1 for Pin configured as on output 
	__REG32(GPIO_BASE + GAFR1_L) = 0x00008010; // set AF34, AF39 for general purpose I/O, alternate function
	__REG32(CLOCK_MANAGER_BASE + CKEN) = 0x0040 ; // Clock enable register. set 6 bit for 1 to clock unit enable

	/*	          DLAB = 0           */
	/*
	Stop bit : 1 bit
	Parity bit : None
	Data bit : 1 bit.  WLS0, WLS1 set 1.
	*/
	__REG(FFUART_BASE + FFLCR) = 0x07; // Line control Register //  0000 0111
	__REG(FFUART_BASE + FFIER) = 0x40; // Interrupt Enable Register // 0100 0000 
	// not interrupt, so set polling. UART unit enable.  
	__REG(FFUART_BASE + FFFCR) = 0x07; // FIFO control Register // 0000 0111
	// Transmit and Receive FIFO enable, Reset Transmitter and Receiver FIFO set 1. 
	
	/*            DLAB = 1          */

	__REG(FFUART_BASE + FFLCR) = 0x87; // 1000 0111 
	__REG(FFUART_BASE + FFDLL) = 0x60; //SERIAL_SPEED_RATE; //baud rate = 9600 = 0x00000060
	__REG(FFUART_BASE + FFDLH) = 0x00; // Divisor Latch High Register. init to 0.

	__REG(FFUART_BASE + FFLCR) = 0x07; // set DLAB = 0.
/* setup internal structure ... */

	node.baudrate[0] = BAUD_9600;
	node.baudrate[1] = BAUD_9600;
	node.uid[COM0]   = NONE;
	node.uid[COM1]   = NONE;
}

/* -- serial_open -------------------------------------------------------------
 *
 * Description : open the serial device driver
 * 
 * Parameters  : unsigned major - DEVICE_SEGMENT_E7T 
 *             : unsigned com - COM0 | COM1
 * Return      : if (success) return UART0_BASE or UART1_BASE 
 *                  else if (alreadyopen) return DEVICE_IN_USE
 *                  else if (unknown) return DEVICE_NEXT
 * Notes       : 
 *
 */

UID serial_open(unsigned int major,unsigned int com) 
{
  if (major==DEVICE_SERIAL_E7T)
  {
    if (com==COM0 || com==COM1)
    {
      if (node.uid[com]!=NONE) 
      {
      return DEVICE_IN_USE;
      } 
      else
      {
      node.uid[com] = uid_generate ();	
      return node.uid[com];  
      }
    } 
  }
    
return DEVICE_NEXT;
}

/* -- serial_close ------------------------------------------------------------
 *
 * Description : close serial device driver
 * 
 * Parameters  : UID id 
 * Return      : 
 *   DEVICE_SUCCESS - successfully close the device
 *   DEVICE_UNKNOWN - couldn't identify the UID
 * Notes       : 
 *
 */

int serial_close(UID id) 
{
  if (node.uid[COM0]==id) 
  { 	
  node.uid[COM0] = NONE;
  return DEVICE_SUCCESS;
  }

  if (node.uid[COM1]==id) 
  { 	
  node.uid[COM1] = NONE;
  return DEVICE_SUCCESS;
  }

return DEVICE_UNKNOWN;
}

/* -- serial_write_byte -------------------------------------------------------
 *
 * Description : waits for response from hardware and writes a 
 *               byte to the device 
 * 
 * Parameters  : UART port address
 *             : BYTE data
 * Return      : none...
 *
 * Notes       : an example of a led write bit
 *
 */

void internal_serial_write(unsigned int port,BYTE c)		
{
	while((__REG(port + FFLSR) & 0x20) == 0x0); // Data not exist in FIFO. check 5 bit(TDRQ) for empty.
	
	__REG(port +FFTHR) = ((u32)c & 0xFF); // Store one character in THR 
}

/* -- serial_write_byte -------------------------------------------------------
 *
 * Description : write a byte to a serial device 
 * 
 * Parameters  : UID device
 *             : BYTE data
 * Return      : none...
 *
 * Notes       : an example of a led write bit
 *
 */

void serial_write_byte(UID id,BYTE data) 
{

  if (node.uid[COM0]==id) 
  {
  internal_serial_write (FFUART_BASE,data);
  return;
  }

  if (node.uid[COM1]==id) 
  {
  internal_serial_write (FFUART_BASE,data);
  return;
  }

}

/* -- internal_serial_read ----------------------------------------------------
 *
 * Description : getkey routine
 *
 * Parameters  : unsigned int port - port address
 * Return      : BYTE - character
 * Notes       : 
 *
 */	
	
BYTE internal_serial_read(unsigned int port)
{

	while ( (__REG32(port + FFLSR) & 0x01) == 0 ) {;}

	return __REG8(port + FFRBR);
}	

/* -- serial_read_byte --------------------------------------------------------
 *
 * Description : read a particular byte value
 * 
 * Parameters  : UID id - COM0 | COM1 
 * Return      : BYTE - read byte otherwise 255
 *
 * Notes       :
 */

BYTE  serial_read_byte(UID id) 
{
  if (node.uid[COM0]==id) 
  {
  return internal_serial_read (FFUART_BASE);
  }
 
  if (node.uid[COM1]==id) 
  {
  return internal_serial_read (FFUART_BASE);
  }

return 255;
}

