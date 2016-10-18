// This is a look up table for basic key strokes. 
// This translates from ps/2 to ascii. 
// The table starts at 0x15, so you should shift 
// the table by 0x15 when you reference it. 
// 
unsigned char ps2_to_ascii[] = {
	'q','1',0x00,0x00,0x00,'z','s','a','w','2',0x00,

	0x00,'c','x','d','e','4','3',0x00,0x00,' ','v','f','t','r','5',0x00,0x00,
	
	'n','b','h','g','y','6',0x00,0x00,0x00,'m','j','u','7','8',0x00,
	
	0x00,',','k','i','o','0','9',0x00,0x00,'.',0x00,'l','p'};


volatile int RCGCUART    __attribute__((at(0x400FE618)));
volatile int UART1BASE   __attribute__((at(0x4000D000)));
volatile int GPIOBCTL    __attribute__((at(0x4000552C)));
volatile int UART1DR     __attribute__((at(0x4000D000)));
volatile int UART1FR     __attribute__((at(0x4000D018)));
volatile int UART1IBRD	__attribute__((at(0x4000D024)));
volatile int UART1FBRD	__attribute__((at(0x4000D028)));
volatile int UART1LCRH	__attribute__((at(0x4000D02C)));
volatile int UART1CTL	__attribute__((at(0x4000D030)));	
volatile int PB_DATA_R __attribute__((at(0x400053FC))); 
volatile int PB_GPIOCR_R   __attribute__((at(0x40005524))); 
volatile int PB_DIR_R  __attribute__((at(0x40005400))); 
volatile int PB_AF_R   __attribute__((at(0x40005420))); 
volatile int PB_AM_R   __attribute__((at(0x40005528))); 
volatile int PB_DEN_R  __attribute__((at(0x4000551C))); 
volatile int PB_IEV_R   __attribute__((at(0x4000540C))); 
volatile int PB_IBE_R   __attribute__((at(0x40005408))); 
volatile int PB_IS_R   __attribute__((at(0x40005404))); 
volatile int PB_IM_R   __attribute__((at(0x40005410))); 
volatile int PB_ICR_R   __attribute__((at(0x4000541C))); 
volatile int PF_DATA_R __attribute__((at(0x400253FC))); 
volatile int PF_DIR_R  __attribute__((at(0x40025400))); 
volatile int PF_AF_R   __attribute__((at(0x40025420))); 
volatile int PF_AM_R   __attribute__((at(0x40025528))); 
volatile int PF_PUR_R   __attribute__((at(0x40025510))); 
volatile int PF_GPIOCR_R   __attribute__((at(0x40025524))); 
volatile int PF_IEV_R   __attribute__((at(0x4002540C))); 
volatile int PF_IBE_R   __attribute__((at(0x40025408))); 
volatile int PF_IS_R   __attribute__((at(0x40025404))); 
volatile int PF_IM_R   __attribute__((at(0x40025410))); 
volatile int PF_ICR_R   __attribute__((at(0x4002541C))); 
volatile int PF_DEN_R  __attribute__((at(0x4002551C))); 
volatile int PF_LOCK_R  __attribute__((at(0x40025520))); 
volatile int RCGC2_R   __attribute__((at(0x400FE108))); //value for RCGC2 to enable clock for port F
volatile int UART1ENBB_R __attribute__((at(0x421A0600)));  // bitbanded enable bit for UART1
volatile int NVIC_PRI0_R __attribute__((at(0xE000E400)));  // GPIOB
volatile int NVIC_PRI9_R __attribute__((at(0xE000E41C)));  // GPIOF
volatile int NVIC_EN0_R  __attribute__((at(0xE000E100)));  // Enable NVIC interrupts
#define RCGC2_PBF 0x00000022    // Enable Port F and Port B
#define UNLOCK 0x4C4F434B       // Port F unlock
	
// Declare Global variables here
volatile uint32_t DONTREAD = 0;

void GPIO_INIT(void) {
	RCGC2_R |= RCGC2_PBF;    // enable clocks
	
	// Initialize Global variables here  
	DONTREAD = 0;
	
	// Unlock Port F
	PF_LOCK_R = UNLOCK;

	// GPIOCR
	PF_GPIOCR_R |= 0x11;  // allow changes to GPIOF[4] and [0] for AFSEL, DEN
	PB_GPIOCR_R |= 0xF;   // allow changes to GPIOB[3:0] for AFSEL, DEN
	// Port B  GPIODIR
	// [1:0] are outputs, [3:2] are inputs
	PB_DIR_R &= ~0xC;
	// Pull-Up resistors on switches
	PF_PUR_R |= 0x11;    
	// Port F  [4] and [0] are inputs
	PF_DIR_R &= ~0x11;
	// Disable alternative function
	PB_AF_R &= ~0xF;
	PF_AF_R &= ~0x11;
	
	// AMSEL
	PB_AM_R &= ~0xF;
	PF_AM_R &= ~0x11;
	
	// Digital Enable
	PB_DEN_R |= 0xF;
	PF_DEN_R |= 0x11;
	
	// UART1
	RCGCUART |= 0x2;
	
	// GPIOCTL PMC1 & PMC0 set to 1 to select UART1
	GPIOBCTL |= 0x11;

	UART1ENBB_R = 0x0;   // disable while configuring UART1 registers
	UART1IBRD |= 0x68;
	UART1FBRD |= 0xB;
	UART1LCRH |= 0x70
	// UART1CTL |= 0x1;
  UART1ENBB_R = 0x1;  // same as above line
		
	// Set Negedge trigger on Port F switches
	
	// Set Negedge trigger on Port B[2] (keyboard clk)
	
	// Not both edges
	
	// clear flag
	
	// arm interrupt
	
	// set NVIC priority
	
	// enable NVIC interrupt
	
}

void UART1_INIT(void) {

	
	
}
	
	
	PB_DIR_R      

}	

	
	// Everything below is garbage
	unsigned char *PB = (unsigned char *) 0x40005000;  // PB[1:0] for UART1 PB[3] input (data) PB[2] interrupt (clk) 
unsigned char *PF = (unsigned char *) 0x40025000;  // use port F0 for SW2, F4 (SW1)

unsigned int *SYSCTL_int = (unsigned int *) 0x400FE000;

// #define RCC_13Mhz  	0x7400540;
#define RCC_16Mhz   0x078E3AD1;   // Default

void SYSCLK()
{
	// 0. set sysclk to main osc
  SYSCTL_int[0x060/4] = RCC_16Mhz;  // 
	SYSCTL_int[0x608/4] |= 0x22;      // RCGCGPIO enable Port B and Port F
}

void GPIO_Init() {
  PF[0x528] = 0x0;   // Disable analog func AMSEL
	PF[0x52C] = 0x0;   // Use GPIOF as GPIO
	
}
	void setup_DH_pins()
{
	//Unlock
	PC[0x520]=0x4B;
	PC[0x521]=0x43;
	PC[0x522]=0x4F;
	PC[0x523]=0x4C;
	//PC[4:7] as output
	PC[0x400] |= 0xF0;
	//PC[4:7] as GPOI
	PC[0x420] &= 0x0F;
	
	//PC[4:7] enable
	PC[0x51C] |= 0xF0;
}

void setup_DL_pins()
{
	//Unlock
	PE[0x520]=0x4B;
	PE[0x521]=0x43;
	PE[0x522]=0x4F;
	PE[0x523]=0x4C;
	//PE[0:3] as output
	PE[0x400] |= 0x0F;
	//PE[0:3] as GPOI
	PE[0x420] &= 0xF0;
	
	//PE[0:3] enable
	PE[0x51C] |= 0x0F;
}

void writeChar(unsigned char CHR)
{
	PA[0x3FC] &= 0x7F;
	
	PE[0x3FC] = CHR;	//DATABUS = DL
	PC[0x3FC] = CHR;	//DATABUS = DH
	
	PA[0x3FC] |= 0x80;
	
}
void setup_wr_rd_pins()
{
	//Unlock
	PA[0x520]=0x4B;
	PA[0x521]=0x43;
	PA[0x522]=0x4F;
	PA[0x523]=0x4C;
	//PA[7] as output
	PA[0x400] |= 0x80;
	//PA[7] as GPOI
	PA[0x420] &= 0x7F;
	
	//PA[7] enable
	PA[0x51C] |= 0x80;
}

void write()
{
	while(1)
	{
		writeChar(0x48);
		writeChar(0x65);
		writeChar(0x6C);
		writeChar(0x6C);
		writeChar(0x6F);
		writeChar(0x21);
		writeChar(0x0A);
	}
}


int main(void)
{
	SYSCLK();
	setup_wr_rd_pins();
	setup_DL_pins();
	setup_DH_pins();
	write();
	
	return 0;
}
