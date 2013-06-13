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
* Module      : tick_service.c
* Description : Header File
* Platform    : Evaluator7T
* History     : 
*
* 2000-03-25 Andrew N. Sloss
* - implemented a tick service
*
*****************************************************************************/


/*****************************************************************************
* IMPORT
*****************************************************************************/

#include "../headers/macros.h"

#include "../../headers/api_types.h"

/*****************************************************************************
* MACRO'S
*****************************************************************************/

#define	__REG32	*(volatile unsigned long *)

//Interrupt Control Registers
#define	INTERRUPT_CONTROL_BASE 0x40D00000

#define	ICIP		0x00
#define	ICMR		0x04
#define	ICLR		0x08
#define	ICCR		0x14

//OS Timer Registers
#define TIMER_BASE	0x40A00000

#define OSMR0		0x00
#define OSCR		0x10
#define OIER		0x1C
#define OSSR		0x14

/*****************************************************************************
* GLOBAL
*****************************************************************************/

UINT countdown;

/*****************************************************************************
* EXTERN's
*****************************************************************************/

/* none... */

/*****************************************************************************
* GLOBALS
*****************************************************************************/

/* none... */

/*****************************************************************************
* ROUTINES
*****************************************************************************/

/* -- eventTickInit -----------------------------------------------------------
*                                                            
* Description  : Initialises the counter timer in milliseconds
*
* Parameters   : UINT msec - sets periodic timer in milliseconds
* Return       : none...
* Notes        : none...
*                                                                             
*/

void eventTickInit (UINT msec)
{
/* ----------------------------------------------------------------------
*
* Initalize the Tick hardware on the Samsung part.
*
* ----------------------------------------------------------------------
	*/
	__REG32(INTERRUPT_CONTROL_BASE + ICCR) =  0x00000001; // Disable Idle Mask = DIM = idel mode on : bit 0 -> 1
	__REG32(INTERRUPT_CONTROL_BASE + ICMR) |= 0x04000000; // TIMTER 0 MASK : bit 26 -> 1
	__REG32(INTERRUPT_CONTROL_BASE + ICLR) &= 0x00000000; // IRQ mode : IL26 = 0
	
	__REG32(TIMER_BASE + OSCR) =  0x00000000; // Count Clear
	__REG32(TIMER_BASE + OSSR) =  0x00000001; // Timer Status Clear
	
											  /* ----------------------------------------------------------------------
											  *
											  * Set the countdown value depending on msec.
											  *
											  * ----------------------------------------------------------------------
	*/
	
	switch (msec)
	{
	case 2: /* fast ... */
		__REG32(TIMER_BASE + OSMR0) = 368640; // 0.1sec -> 368640
		break;
	default: /* slow ... */
		__REG32(TIMER_BASE + OSMR0) = 3686400; // 1sec -> 3686400
		break;	
	}
}

/* -- eventTickService --------------------------------------------------------
*
* Description : interrupt service routine for timer0 interrupt.
*
* Parameters  : none...
* Return      : none...
* Notes       : 
* 
* timer interrupt everytime the counter reaches 0. To reset
* the timer TDATA0 has to have a new initialization value.
* Finally the last act is to unmask the timer interrupt on 
* the Samsung KS3250C100.
*
*/ 

#define tIOPDATA    (volatile unsigned int *)(SYSCFG + 0x5008)
#define LEDBANK     *tIOPDATA


#define LED_4_ON    LEDBANK |= 0x00000010
#define LED_4_OFF   LEDBANK = LEDBANK &~ 0x00000010

unsigned int xLED = 0;


void eventTickService(void) 
{ 
	
	/* -- reset timer interrupt... */ 
	disable_irq(); // Disable IRQ	
	
	if((__REG32(INTERRUPT_CONTROL_BASE + ICIP) & 0x04000000) != 0x00000000){ // pin 26 : OS Timer 0 // if IRQ set, bit 26 set 1. So if it's value same with 0x00000000, not set IRQ
		__REG32(TIMER_BASE + OSCR) =  0x00000000; // OSCR = 0
		__REG32(TIMER_BASE + OSSR) &= 0x00000000; // OSSR : 0 -> 1 
		__REG32(TIMER_BASE + OSSR) |= 0x00000001; // if OSMR and OSCR have same value, OSSR set bit 0 : 1
	}
	/* -- unmask the interrupt source.... */
	
	enable_irq(); // IRQ Enable
	
	//*(volatile unsigned int*)INTMSK &= ~((1<<INT_GLOBAL)|(1<<10)|(1<<0));
} 

/* -- eventTickStart ----------------------------------------------------------
*
* Description  : switches on the periodic tick event
*
* Parameters   : none...
* Return       : none...
* Notes        : none...
*
*/  

void eventTickStart(void)
{	
	//*TDATA0   = countdown; /* load Counter Timer ... */
	//*TMOD     |= 0x1;      /* enable interval interrupt ... */
	__REG32(TIMER_BASE + OSCR) =  0x00000000; // Count Clear
	__REG32(TIMER_BASE + OIER) |= 0x00000001; // TIMER 0 : set bit 0 -> 1
	__REG32(TIMER_BASE + OSSR) =  0x00000001; // Timer Status Clear
	
	/* -- unmask the interrupt source.. */ 
	
	//*(volatile unsigned int*)INTMSK &= ~((1 << INT_GLOBAL) | (1<<10) | (1<<0));     
}


void enable_irq(void){
	__asm__ __volatile__("mrs	r1, cpsr");			// transfer the contents of cpsr into a r1
	__asm__ __volatile__("bic	r1, r1, #0x80");	// initialize r1 to 0x80 
	__asm__ __volatile__("msr	cpsr, r1");			// transfer the contents of r1 the cpse
}

void disable_irq(void){
	__asm__ __volatile__("mrs	r1, cpsr");			// transfer the contents of cpsr into a r1
	__asm__ __volatile__("orr	r1, r1, #0x80"); 	// or r1 and 0x80
	__asm__ __volatile__("msr	cpsr, r1");   		// transfer the contents of r1 the cpse
}

