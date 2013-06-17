
#include "../headers/reg.h"
#include "../headers/lcd_reg.h"

#define FBDEVFILE "/dev/fb"
///////////////////


void controller_setting(void)
{

/* LCD Controller Control Register */
__REG(LCCR_BASE + LCCR0) |= 0x000000f9;
__REG(LCCR_BASE + LCCR1) |= 0x06020cef;
__REG(LCCR_BASE + LCCR2) |= 0x0102053f;
__REG(LCCR_BASE + LCCR3) |= 0x04700005;

/* DMA Frame Branch Registers */

}
