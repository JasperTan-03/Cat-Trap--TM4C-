#include <stdint.h>
#include "../inc/tm4c123gh6pm.h"
#include "TExaS.h"
#include "ST7735.h"
#include "Board.h"


void DisableInterrupts(void); // Disable interrupts
void EnableInterrupts(void);  // Enable interrupts
uint32_t ADC0MailFn(void);
void placeCat(void);
uint8_t homeScreenStatus;
uint8_t checkADC1(void);
uint8_t checkADC0(void);

uint8_t printBlueFlag;
uint8_t resetFlag;


void Button_init(void){

    volatile uint32_t delay;
    SYSCTL_RCGCGPIO_R |= 0x10; //turn on clock for port F, E, and C 0001 0000
    delay = SYSCTL_RCGCGPIO_R;
		delay = SYSCTL_RCGCGPIO_R;
    GPIO_PORTE_DEN_R |= 0x07; //0000 0011
    GPIO_PORTE_DIR_R &= ~(0x07); //0000 0011
}



//volatile uint32_t risingEdges = 0;

void EdgeCounter_Init(void){       

  SYSCTL_RCGCGPIO_R |= 0x00000010; // (a) activate clock for port F

//  risingEdges = 0;             // (b) initialize count and wait for clock
  GPIO_PORTE_DIR_R &= ~0x07;    // (c) make PE0 input
  GPIO_PORTE_DEN_R |= 0x07;     //     enable digital I/O on PE0
  GPIO_PORTE_IS_R &= ~0x07;     // (d) PE0 is edge-sensitive
  GPIO_PORTE_IBE_R &= ~0x07;    //     PE0 is not both edges
  GPIO_PORTE_IEV_R &= 0x07;    //     PE0 rising edge event
  GPIO_PORTE_ICR_R = 0x07;      // (e) clear flag0
  GPIO_PORTE_IM_R |= 0x07;      // (f) arm interrupt on PE0
  NVIC_PRI1_R = (NVIC_PRI1_R & 0xFFFFFF1F)|0x00000040; // (g) priority 2 
  NVIC_EN0_R = 0x00000010;      // (h) enable interrupt 4 in NVIC
  EnableInterrupts();           // (i) Enable global Interrupt flag (I)

}

// Initialize Port F so PF1, PF2 and PF3 are heartbeats


void PortF_Init(void){
	
		
	
  volatile int delay;
  SYSCTL_RCGCGPIO_R |= 0x20;
  delay = SYSCTL_RCGCGPIO_R;
  GPIO_PORTF_DIR_R |= 0x0E;
  GPIO_PORTF_DEN_R |= 0x0E;
}


void GPIOPortE_Handler(void){
  if ((GPIO_PORTE_RIS_R & 0x1) == 0x1){ 										//PE0 0000 0110
		uint32_t ADC0Mail = ADC0MailFn();
		
		if ((ADC0Mail > 0 ) && (ADC0Mail <= 1365)){
	//	ST7735_FillScreen(ST7735_WHITE); //just for test		
			homeScreenStatus = 2; //start game
			
		}

		else if ((ADC0Mail > 1370 ) && (ADC0Mail <= 2730)){
			//ST7735_FillScreen(ST7735_RED);
			homeScreenStatus = 0; //Englsih
		}

		else if((ADC0Mail > 2740 )){
			//ST7735_FillScreen(ST7735_GREEN);
			homeScreenStatus = 1; //Spanish
		}
	}
	
  /*if (((GPIO_PORTE_DATA_R & 0x0002) == 0x0002) || ((GPIO_PORTE_DATA_R & 0x0002) == 0x0021)){  										//PE1
		GPIO_PORTE_ICR_R = 0x02;      // acknowledge flag1
		printBlueFlag = 1;
	}
}*/
	if ((GPIO_PORTE_RIS_R & 0x2) == 0x2){
		printBlueFlag = 1;
	}
	if ((GPIO_PORTE_RIS_R & 0x4) == 0x4){
		resetFlag = 1;
	}
	GPIO_PORTE_ICR_R = 0x07;      // acknowledge flag1
}

uint8_t homeStatus(void){

		return homeScreenStatus;
}



