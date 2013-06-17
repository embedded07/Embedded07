
/* LCD Controller Control Registers */
#define LCCR_BASE	0x44000000
#define LCCR0 		0x00
#define LCCR1 		0x04
#define LCCR2 		0x08
#define LCCR3 		0x0c

/* DMA Frame Branch Registers */
#define FBR_BASE		0x44000000
#define FBR0 			0x20 /* DMA Channel0 Frame Branch Registers */
#define FBR1			0x24 /* DMA Channel1 Frame Branch Registers */

/* LCD Controller Rgister */
#define LCSR	0x44000038 /* LCD Controller Staus Rgister */
#define LIIDR	0x4400003c /* LCD Controller Interrupt ID Register */

/* TMED Register */
#define TRGBR	0x44000040 /* TMED RGB Seed Register */
#define TCR		0x44000044 /* TMED Control Register */

/* DMA Channel0 Register */
#define FDADR0	0x44000200 /* DMA Channel0 Frame Descriptor Address Register */
#define FSADR0	0x44000204 /* DMA Channel0 Frame Descriptor Address Register */
#define FIDR0	0x44000208 /* DMA Channel0 Frame ID Register */
#define LDCMD0	0x4400020c /* DMA Channel0 Command Register */

/* DMA Channel1 Register */
#define FDADR1	0x44000210 /* DMA Channel1 Frame Descriptor address Register */
#define FSADR1	0x44000214 /* DMA Channel1 Frame Source Address Register */
#define FIDR1	0xf4000218 /* DMA Channel1 Frame ID Register */
#define LDCMD1	0x4400021c /* DMA Channel1 Command Register */


