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
* Module      : led_driver.c
* Description : device driver for led on the LDS2000
* OS          : SLOS 0.09
* Platform    : LDS2000
* History     :
*
*****************************************************************************/

/*****************************************************************************
* IMPORT
*****************************************************************************/

#include "../../devices/ddf_frame.h"
#include "led_driver.h"

/*****************************************************************************
* MACROS
*****************************************************************************/

#define __REG *(volatile unsigned long *)

/* -- base register */

#define MSC_BASE	0x48000000 // MSC_BASE
#define MSC1		0x0000000c // MSC1
#define MSC_VALUE	0x00005aa8 // 

#define LED_VALUE 0x08000008
#define LED_PORT	__REG(LED_VALUE)

/* -- set */

#define LED_1_ON    (LED_PORT = LED_PORT | 0x00000040)
#define LED_2_ON    (LED_PORT = LED_PORT | 0x00000030)
#define LED_3_ON    (LED_PORT = LED_PORT | 0x00000020)
#define LED_4_ON    (LED_PORT = LED_PORT | 0x00000010)
#define LED_5_ON    (LED_PORT = LED_PORT | 0x00000004)
#define LED_6_ON    (LED_PORT = LED_PORT | 0x00000003)
#define LED_7_ON    (LED_PORT = LED_PORT | 0x00000002)
#define LED_8_ON    (LED_PORT = LED_PORT | 0x00000001)

/* -- reset */

#define LED_1_OFF   (LED_PORT = LED_PORT & ~0x00000040)
#define LED_2_OFF   (LED_PORT = LED_PORT & ~0x00000030)
#define LED_3_OFF   (LED_PORT = LED_PORT & ~0x00000020)
#define LED_4_OFF   (LED_PORT = LED_PORT & ~0x00000010)
#define LED_5_OFF   (LED_PORT = LED_PORT & ~0x00000004)
#define LED_6_OFF   (LED_PORT = LED_PORT & ~0x00000003)
#define LED_7_OFF   (LED_PORT = LED_PORT & ~0x00000002)
#define LED_8_OFF   (LED_PORT = LED_PORT & ~0x00000001)

/*****************************************************************************
* DATATYPES
*****************************************************************************/

typedef struct 
{
	unsigned char led[8]; 
	unsigned char uid[8];
} internal_ledstr;

/*****************************************************************************
* STATICS
*****************************************************************************/

internal_ledstr		diodes;

/* -- led_init ----------------------------------------------------------------
*
* Description : initalize the LED device driver internal 
*               data structures and hardware. Set all the
*               LED's to be zero.
* 
* Parameters  : none...
* Return      : none...
* Notes       : none...
*
*/

void led_init(void) 
{
	
	/* switch off all the LED's ....................... */
	int i;

	__REG(MSC_BASE + MSC1) = MSC_VALUE;
	
	LED_1_OFF;
	LED_2_OFF;
	LED_3_OFF;
	LED_4_OFF;
	LED_5_OFF;
	LED_6_OFF;
	LED_7_OFF;
	LED_8_OFF;
	
	for (i = 0 ; i < 8 ; i++){
		diodes.led[i] = OFF;
		diodes.uid[i] = NONE;
	}	
	
}

/* -- led_open ----------------------------------------------------------------
*
* Description : example open on the LED 
* 
* Parameters  : unsigned major = 55075 
*             : unsigned minor = (0 = LED1, 1 = LED2, ... , 7 = LED8)
* Return      : UID -
* Notes       : 
*
*/

UID led_open(unsigned int major, unsigned int minor) 
{
	
	if (major == DEVICE_LED_LDS2000)
	{
		if (minor < 8) 
		{
			if (diodes.uid[minor] == NONE)
			{
				diodes.uid[minor] = uid_generate();
				return diodes.uid[minor];      /* unique ID */
			}
			else
			{
				return DEVICE_IN_USE;	
			}
		} 
		else
		{
			return DEVICE_UNKNOWN;	
		}
	}
	
	return DEVICE_NEXT;
}

/* -- led_close ---------------------------------------------------------------
*
* Description : example open on the LED 
* 
* Parameters  : UID id 
* Return      : 
*
*   DEVICE_SUCCESS - successfully close the device
*   DEVICE_UNKNOWN - couldn't identify the UID
*
* Notes       : 
*
*/

int led_close(UID id) 
{
	int x;
	
	for (x = 0 ; x < 8 ; x++)
	{
		if (diodes.uid[x] == id) 
		{
			diodes.uid [x] = NONE;
			return DEVICE_SUCCESS;
		}
	}
	
	return DEVICE_UNKNOWN;
}

/* -- led_write_bit -----------------------------------------------------------
*
* Description : write a particular bit to an LED 
* 
* Parameters  : UID id = 55075 + unit (0..3)
*	     	: BYTE led_set - least significant bit is used
* Return	     : none...
*
* Notes       : an example of a led write bit
*
*/

void led_write_bit(UID id,BYTE led_set) 
{
	int x;
	
	for (x = 0 ; x < 8 ; x++)
	{
		if ((diodes.uid[x] == id) && (diodes.uid[x] != NONE))
		{
			led_set &= 1; /* bitwise AND */
			diodes.led [x] = led_set;
		}
	}
}

/* -- led_read_bit ------------------------------------------------------------
*
* Description : read a particular bit value 
* 
* Parameters  : UID id = 55075 + unit (0..3)
* Return      : value return error if 255
*
* Notes       : an example of a led read bit
*/

BYTE led_read_bit(UID id) 
{
	int x;
	
	for (x = 0 ; x < 8 ; x++)
	{
		if ((diodes.uid[x] == id) && (diodes.uid[x] != NONE))
		{    
			return diodes.led [x];		
		}
	}
	/* 
	* error correction goes here...
	*/
	
	return 255;
}

