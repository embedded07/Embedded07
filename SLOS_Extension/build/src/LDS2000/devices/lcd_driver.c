/*****************************************************************************
 * IMPORT
 *****************************************************************************/

#include "../headers/reg.h"
#include "../headers/lcd_reg.h"

/*****************************************************************************
 * MACROS
 *****************************************************************************/

#define PDD 0x000FF000

/* GPIO Alternate Fuctions
ALL AF2_OUT (AF Encoding :10)

[GP58:GP73] 	LCD Controller		(LCD data pin 0~15) 
[GP74]			LCD Frame Clock	(LCD_FCLK)
[GP75]			LCD Line Clock		(LCD_LCLK)
[GP76]			LCD Pixel Clock 	(LCD_PCLK)
[GP77]			LCD AC Bias 		(LCD_ACBIAS)

*/

void lcd_init(void) 
{

	/* Initialize GPIO */
	__REG(GPIO_BASE + GPDR1) &= 0xFFFFFFFB;
	__REG(GPIO_BASE + GPDR1) |= 0x80;

	__REG(GPIO_BASE + GPDR2) &= 0xFFFFFFFB;
	__REG(GPIO_BASE + GPDR2) |= 0x80;

	__REG(GPIO_BASE + GAFR1_U) = 0xAAA00000;
	__REG(GPIO_BASE + GAFR2_L) = 0x082AAAAA;

}

void enable_controller(void) /* FDADRx LCCR0 */
{
	__REG(FDADR0) = 0x00;
	__REG(LCCR_BASE + LCCR0) = 0x00000483;
	__REG(LCCR_BASE + LCCR0) += PDD;

}

void disable_controller(void) /* LCCR0 LCSR */
{
	__REG(LCSR) = 0x00000000;
}

void resetting_controller(void) /* GPIO */
{
}

void input_FIFO(void)
{
}

void lookup_palette(void)
{
}

void tmed_dithering(void)
{
}

void output_FIFO(void)
{
}

void passive_display_timing(void)
{
}

void active_display_timing(void)
{
}

void pixel_data(void)
{
}

void dma_setting(void)
{
}


