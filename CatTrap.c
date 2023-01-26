// CatTrap.c
// Runs on TM4C123
// Rohan Jain and Jasper Tan
//The University of Texas at Austin EE 319K
// Last Modified: 5/3/2022 
// Based on https://llerrah.com/cattrap.htm
// https://opengameart.org/content/rpg-gui-selection-arrow


// ******* Possible Hardware I/O connections*******************
// Slide pot pin 1 connected to ground
// Slide pot pin 2 connected to PD2/AIN5
// Slide pot pin 3 connected to +3.3V 
// buttons connected to PE0-PE3
// 32*R resistor DAC bit 0 on PB0 (least significant bit)
// 16*R resistor DAC bit 1 on PB1
// 8*R resistor DAC bit 2 on PB2 
// 4*R resistor DAC bit 3 on PB3
// 2*R resistor DAC bit 4 on PB4
// 1*R resistor DAC bit 5 on PB5 (most significant bit)
// LED on PD1
// LED on PD0


#include <stdint.h>
#include "../inc/tm4c123gh6pm.h"
#include "ST7735.h"
#include "Print.h"
#include "Random.h"
#include "TExaS.h"
#include "ADC.h"
#include "Images.h"
#include "Sound.h"
#include "Timer1.h"
#include "Timer0.h"
#include "Button.h"
#include "Slicer.h"
#include "Board.h"


#define PF1       (*((volatile uint32_t *)0x40025008))
#define PF2       (*((volatile uint32_t *)0x40025010))
#define PF3       (*((volatile uint32_t *)0x40025020))
	
#define yellow 0
#define grey 1
#define cat  2
#define selected 3
#define border 4

void DisableInterrupts(void); // Disable interrupts
void EnableInterrupts(void);  // Enable interrupts
void HomeScreen(uint8_t language);
uint16_t gravity(uint16_t height,uint16_t velocity);;
void Timer0A_Handler(void);
void Random81(uint8_t ammount, uint32_t *pointer);
uint32_t ADC0MailFn(void);
uint32_t ADC1MailFn(void);
void selector(void);

uint8_t checkADC1(void);
uint8_t checkADC0(void);

void Timer1A_Handler(void);
void Timer1B_Handler(void);

void Distance_Init (void);
uint8_t GetCatRow (void);
uint8_t GetCatCol (void);
uint8_t CatWhere (void);
void moveCat(uint8_t dir);

void EndGame (uint8_t result, uint8_t language);

extern uint8_t printBlueFlag; // from Button.c
extern uint8_t homeScreenStatus;
void Sound_Off(void);

uint32_t ADC0Status;
uint32_t ADC0Mail;

uint32_t ADC1Status;
uint32_t ADC1Mail;

uint8_t selectedX;
uint8_t selectedY;
uint8_t firstFlag;
uint8_t previousX; 
uint8_t previousY;

uint8_t gameFlag;
uint16_t xMail;
uint16_t yMail;
uint8_t limit;
uint8_t SquareCounter;
uint8_t squareFlag;
extern int8_t musicSelect;
extern uint8_t resetFlag; //resets game if set to 1

uint8_t Board [9][9] = {{0, 0, 0, 0, 0, 0, 0, 0, 0},
												{0, 0, 0, 0, 0, 0, 0, 0, 0},
												{0, 0, 0, 0, 0, 0, 0, 0, 0},
												{0, 0, 0, 0, 0, 0, 0, 0, 0},
												{0, 0, 0, 0, 0, 0, 0, 0, 0},
												{0, 0, 0, 0, 0, 0, 0, 0, 0},
												{0, 0, 0, 0, 0, 0, 0, 0, 0},
												{0, 0, 0, 0, 0, 0, 0, 0, 0},//8
												{0, 0, 0, 0, 0, 0, 0, 0, 0},
											};

#define Amount 11
uint32_t TotalG[Amount];
											
#define N 0 //NORTH
#define E	1 //EAST
#define S 2 //SOUTH
#define W 3 //WEST
#define T 4	//TRAPPED
#define L 5 //LOST



	uint8_t Distance[4]; //measured in squares away from border (North, East, South, West)
	//uint8_t ClosestBorder; //North,East,South,West
	uint8_t AdjSquare[4]; //how many grey blocks in adjacent grey blocks (North, East, South, West)
	uint8_t Available[4]; //set to 1 if it's not grey or square is available
	uint8_t Quad[4]; // 0: top right, 1: top left, 2: bottom left, 3: bottom right
	uint8_t outcome;

	uint8_t language;
	uint8_t score;
											
int main(void){
	makeBlack(CatTrap, 97, 20);
	makeBlack(EnglishTrapped, 100, 35);
	makeBlack(SpanishTrapped, 100, 35);
	makeBlack(EnglishEscaped, 120, 18);
	makeBlack(SpanishEscaped, 120, 20);
	while(1){
	//INITIALIZATION
  DisableInterrupts();
  TExaS_Init(NONE);       // Bus clock is 80 MHz 
	//ST7735_InitR(INITR_REDTAB);   //<--- is this needed?
	firstFlag = 0;
	gameFlag = 0;
	resetFlag = 0;
  Random_Init(1);
	PortF_Init();
	Button_init();
	EdgeCounter_Init();
	ADC0_Init();
	ADC1_Init();
	Sound_Init();
	Timer1_Init(8000000, 3);
  Output_Init();
	ST7735_SetRotation(3);
	ST7735_FillScreen(0x0000);            // set screen to black
	score = 0;
	musicSelect = 0;
	Random_Init(ADC0Mail);
  Random81(Amount, TotalG);
  EnableInterrupts();
	Sound_Start(11000);
	homeScreenStatus = 0;
	while (homeScreenStatus != 2){ // Home Screen
		language = homeScreenStatus;
		HomeScreen(language);
	}
	
	int RGflag = 1;
	while(RGflag){
		DisableInterrupts();
		Board_Init(Board[0], 9, 9);
		xMail = 23;
		yMail = 0;
		limit = 120;
		SquareCounter = 0;
		TIMER1_CTL_R = 0x00000000;
		Timer0_Init(80000, 3);
		squareFlag = 1;
		ST7735_FillScreen(0x0000);
		EnableInterrupts();
	//PRINT BOARD
	while(squareFlag){
	}
	//REGENERATE BOARD IF RED BUTTON PRESSED
	//CONTINUE IF
		
	DisableInterrupts();
	uint32_t temp;
	temp = GPIO_PORTE_DATA_R;
	while((temp & 0x5) == 0){
		temp = GPIO_PORTE_DATA_R;
	}
		RGflag = (temp & 0x4);
	}
	
	
	TIMER0_CTL_R = 0x00000000;
	Timer1_Init(4000000, 3);
	
	selectedX = checkADC1();
	selectedY = checkADC0();
	EnableInterrupts();
	
	while(1){
		selector();
		//Distance_Init();
		if (printBlueFlag == 1){
			Board_Change(Board[0], selectedY, selectedX, grey);
			ST7735_DrawBitmap((23 + (13*selectedX)), (16 + (13*selectedY)), GreySquare, 10, 10);
			
			score++;
			
			DisableInterrupts();
			Distance_Init();
			outcome = CatWhere();
			if ((outcome == T) || (outcome == L)){
         break;
      }
			
			moveCat(outcome); // should move sprite up down left right
			EnableInterrupts();
			
			//selectedX = checkADC1();
			//selectedY = checkADC0();
			while(((previousX == selectedX) && (previousY == selectedY)) || ((Board[selectedY][selectedX] == grey) || (Board[selectedY][selectedX] == cat))) {
				selectedX = checkADC1();
				selectedY = checkADC0();
			}
			Board_Change(Board[0], selectedY, selectedX, selected);
			ST7735_DrawBitmap((23 + (13*selectedX)), (16 + (13*selectedY)), BlueSquare, 10, 10);
			//firstFlag = 0;
		}
		printBlueFlag = 0;		
	}
	//END GAME
	//make language a global
	//EndGame(score, outcome, language);
		EndGame(outcome, language);
		ST7735_FillScreen(0x0000);            // set screen to black
	}
}

typedef enum {English, Spanish, Portuguese, French} Language_t;
Language_t myLanguage=English;
typedef enum {HELLO, GOODBYE, LANGUAGE} phrase_t;
	const char Hello_English[] ="Hello";
	const char Hello_Spanish[] ="\xADHola!";
	const char Hello_Portuguese[] = "Ol\xA0";
	const char Hello_French[] ="All\x83";
	const char Goodbye_English[]="Goodbye";
	const char Goodbye_Spanish[]="Adi\xA2s";
	const char Goodbye_Portuguese[] = "Tchau";
	const char Goodbye_French[] = "Au revoir";
	const char Language_English[]="English";
	const char Language_Spanish[]="Espa\xA4ol";
	const char Language_Portuguese[]="Portugu\x88s";
	const char Language_French[]="Fran\x87" "ais";
const char *Phrases[3][4]={
  {Hello_English,Hello_Spanish,Hello_Portuguese,Hello_French},
  {Goodbye_English,Goodbye_Spanish,Goodbye_Portuguese,Goodbye_French},
  {Language_English,Language_Spanish,Language_Portuguese,Language_French}
};

	



void HomeScreen(uint8_t language){
	uint16_t pointerY;
	uint16_t pointerX = 45;
	uint8_t returnFlag = 0;
	uint32_t oldADC0; 
	
	DisableInterrupts();
	ST7735_FillScreen(ST7735_BLACK);
	
	ST7735_DrawBitmap(31, 25, CatTrap, 97, 20);
	ST7735_DrawString(11, 7, (char*) Phrases[2][0], ST7735_YELLOW); //English
	ST7735_DrawString(11, 9, (char*) Phrases[2][1], ST7735_YELLOW); //spanish
	
    if(language == 1){ //spanish
			ST7735_DrawString(11, 5, "Empez\xA2", ST7735_YELLOW); 
    }
    else { //english
			ST7735_DrawString(11, 5, "Start ", ST7735_YELLOW);
    }

	//ST7735_DrawBitmap(64, 60, Pointer, 16, 11);
	EnableInterrupts();
		while (returnFlag == 0){	
			if ((ADC0Mail > 0 ) && (ADC0Mail <= 1365)){
				if ((oldADC0 >= 1365)){
					ST7735_DrawBitmap(pointerX, pointerY, blackPointerSquare, 16, 11);
				}
				oldADC0 = ADC0Mail;
				pointerY = 60;
				ST7735_DrawBitmap(pointerX, pointerY, Pointer, 16, 11);
			}
			
			else if ((ADC0Mail > 1370 ) && (ADC0Mail <= 2730)){
				if ((oldADC0 > 2735) || (oldADC0 < 1365)){
					ST7735_DrawBitmap(pointerX, pointerY, blackPointerSquare, 16, 11);
				}
				oldADC0 = ADC0Mail;
				pointerY = 80;
				ST7735_DrawBitmap(pointerX, pointerY, Pointer, 16, 11);
			}
			
			else if((ADC0Mail > 2740 )){
				if ((oldADC0 < 2735)){
					ST7735_DrawBitmap(pointerX, pointerY, blackPointerSquare, 16, 11);
				}
				oldADC0 = ADC0Mail;
				pointerY = 100;
				ST7735_DrawBitmap(pointerX, pointerY, Pointer, 16, 11);
			}
			if (homeStatus() != language){
				returnFlag = 1;
			}
			
		}
		DisableInterrupts();
		return;
}

	


void selector(void){
	
	DisableInterrupts();
		previousX = selectedX;
		previousY = selectedY;
		
		selectedX = checkADC1();
		selectedY = checkADC0();
		
		if (firstFlag == 0){
			Board_Change(Board[0], selectedY, selectedX, selected);
			ST7735_DrawBitmap((23 + (13*selectedX)), (16 + (13*selectedY)), BlueSquare, 10, 10); //draw new
			ST7735_DrawBitmap((21 + (13*4)), (18 + (13* (4))), catdown, 14, 14);
		}
		else	if (((selectedX != previousX) || (selectedY != previousY)) && (firstFlag == 1) && (Board[selectedY][selectedX] != grey) && (Board[selectedY][selectedX] != cat)){
			Board_Change(Board[0], previousY, previousX, yellow);
			ST7735_DrawBitmap((23 + (13*previousX)), (16 + (13*previousY)), YellowSquare, 10, 10);	// erase/redraw0
			
			Board_Change(Board[0], selectedY, selectedX, selected);
			ST7735_DrawBitmap((23 + (13*selectedX)), (16 + (13*selectedY)), BlueSquare, 10, 10); //draw new
		}
		else{
			selectedX = previousX;
			selectedY = previousY;
		}
		firstFlag = 1;
		
		if (printBlueFlag == 1){
			ST7735_DrawBitmap((23 + (13*selectedX)), (16 + (13*selectedY)), BlueSquare, 10, 10); 
		}
		printBlueFlag = 0;
		
		EnableInterrupts();
  }

	

uint8_t checkADC1(void){
	// 9x9 grid, 4096/9 = 455
		uint8_t selectedX;
		//ADC1 selects X direction
		if (ADC1Mail < 410){
			selectedX = 0;
		}
		if (ADC1Mail > 415 && ADC1Mail < 910){
			selectedX = 1;
		}
		if (ADC1Mail > 915 && ADC1Mail < 1365){
			selectedX = 2;
		}
		if (ADC1Mail > 1370 && ADC1Mail < 1820){
			selectedX = 3;
		}
		if (ADC1Mail > 1825 && ADC1Mail < 2275){
			selectedX = 4;
		}
		if (ADC1Mail > 2280 && ADC1Mail < 2730){
			selectedX = 5;
		}
		if (ADC1Mail > 2735 && ADC1Mail < 3185){
			selectedX = 6;
		}
		if (ADC1Mail > 3190 && ADC1Mail < 3645){
			selectedX = 7;
		}
		if (ADC1Mail > 3650){
			selectedX = 8;
		}
		return selectedX;
}

uint8_t checkADC0(void){
			//ADC0 is Y
		uint8_t selectedY;
		if (ADC0Mail < 410){
			selectedY = 0;
		}
		if (ADC0Mail > 415 && ADC0Mail < 910){
			selectedY = 1;
		}
		if (ADC0Mail > 915 && ADC0Mail < 1365){
			selectedY = 2;
		}
		if (ADC0Mail > 1370 && ADC0Mail < 1820){
			selectedY = 3;
		}
		if (ADC0Mail > 1825 && ADC0Mail < 2275){
			selectedY = 4;
		}
		if (ADC0Mail > 2280 && ADC0Mail < 2730){
			selectedY = 5;
		}
		if (ADC0Mail > 2735 && ADC0Mail < 3185){
			selectedY = 6;
		}
		if (ADC0Mail > 3190 && ADC0Mail < 3645){
			selectedY = 7;
		}
		if (ADC0Mail > 3650){
			selectedY = 8;
		}
		return selectedY;
}

void Timer0A_Handler(void){
		PF2^= 0x04;
    TIMER0_ICR_R = 0x01;
    int i = 0;
    for(i = 0; i < 11; i++){
        if(TotalG[i] == SquareCounter){
            ST7735_DrawBitmap(xMail, yMail, GreySquare, 10, 10);
// n*(((2*n)+1)%2) => absolute value
						Board_Change(Board[0], ((SquareCounter-80)*(((2*(SquareCounter-80))+1)%2))/9, (SquareCounter%9), grey);  
					break;
        }
        else if(i == 10){
            ST7735_DrawBitmap(xMail, yMail, YellowSquare, 10, 10);
        }
    }
    ST7735_FillRect(xMail, (yMail-10), 10, 1, ST7735_BLACK);
    if(yMail == limit){
        xMail += 13;
        yMail = 0;
        SquareCounter++;
    }
    else{
        yMail += 1;
    }
    if(xMail >= 137){
        yMail += 13;
        xMail = 23;
        limit -= 13;
    }
    if(limit < 16){
      TIMER0_CTL_R = 0x00000000;
			squareFlag = 0;
    }

}


void Timer1A_Handler(void){ // can be used to perform tasks in background
  TIMER1_ICR_R = TIMER_ICR_TATOCINT;// acknowledge TIMER1A timeout
   // execute user task
		PF1 ^= 0x02;       // Heartbeat
		uint32_t value = ADC0_In();
		if(ADC0Mail != value){
			ADC0Mail = value;
			ADC0Status = 0;
	}
	 
		uint32_t value1 = ADC1_In();
		if(ADC1Mail != value1){
			ADC1Mail = value1;
			ADC1Status = 0;
	}
}


uint32_t ADC0MailFn(void){
	return ADC0Mail;
}

uint32_t ADC1MailFn(void){
	return ADC1Mail;
}


void Random81(uint8_t amount, uint32_t *pointer){
    int i, j, k;
    for(i = 0; i < amount; i++){
        k = Random()%81;
        *(pointer + (i)) = k;
        for(j = i-1; j >= 0; j--){
            if(*(pointer + (i)) == *(pointer + (j)) || *(pointer + (i)) == 40){
                *(pointer + (i)) = Random()%81;
                j = i-1;
            }
        }
    }
}





void Distance_Init (void){
	int i, j, row, col;
	row = GetCatRow();
	col = GetCatCol();
	
	Distance[N] = (row);
	Distance[E] = (8 - col);
	Distance[S] = (8 - row);
	Distance[W] = (col);
	
	AdjSquare[N] = 0;
	AdjSquare[E] = 0;
	AdjSquare[S] = 0;
	AdjSquare[W] = 0;
	
	for(i = 0; i < row; i++){
		if(Board[i][col] == grey)
			AdjSquare[N]++;
	}
	
	for(i = col+1; i < 9; i++){
		if(Board[row][i] == grey)
			AdjSquare[E]++;
	}
	
	for(i = row+1; i < 9; i++){
		if(Board[i][col] == grey)
			AdjSquare[S]++;
	}
	
	for(i = 0; i < col; i++){
		if(Board[row][i] == grey)
			AdjSquare[W]++;
	}
	
	//set to 1 if it's not grey or square is available	
	Available[N] = (Board[row-1][col] == grey)?0:1;
	Available[E] = (Board[row][col+1] == grey)?0:1;
	Available[S] = (Board[row+1][col] == grey)?0:1;
	Available[W] = (Board[row][col-1] == grey)?0:1;
	
	
	//THIS IS NOT FINALIZED
	for(i = 0; i < 4; i++){
		Quad[i]=0;
	}	
	
	//Quad 0
	for(i = 0; i < row; i++){
		for(j = col + 1; j < 9; j++){
			if(Board[i][j] == grey)
				Quad[0]++;
		}
	}
	
	//Quad 1
	for(i = 0; i < row; i++){
		for(j = 0; j < col; j++){
			if(Board[i][j] == grey)
				Quad[1]++;
		}
	}
	
	//Quad 2
	for(i = row + 1; i < 9; i++){
		for(j = 0; j < col; j++){
			if(Board[i][j] == grey)
				Quad[2]++;
		}
	}
	
	//Quad 3
	for(i = row + 1; i < 9; i++){
		for(j = col + 1; j < 9; j++){
			if(Board[i][j] == grey)
				Quad[3]++;
		}
	}
}

//Where the Cat should go? returns N, E, S, W, Trapped(T), Lost
uint8_t CatWhere (void){
	int i, j, AvailCount = 0;
	int8_t points[4]; //counts which direction has more reason to go to (N,E,S,W)
	//if there's more points in one direction, go the opposite direction
	//if there's less points in one direction, go towards that direction
	
	if((GetCatRow() == 0) || (GetCatRow() == 8) || (GetCatCol() == 0) || (GetCatCol() == 8)){
		return L; 
	}
	
	for(i = 0; i < 4; i++){
			points[i] = 0;
	}
	
	for(i = 0; i < 4; i++){
		if(Available[i])
			AvailCount++;	
		else 
			points[i] = -128;
	}
	
	//if no squares are available
	if(AvailCount == 0) return T;
	
	//if 1 squares is available
	if(AvailCount == 1){
		for(i = 0; i < 4; i++){
			if(points[i] == 0) return i;
		}
	}
	
	//if 2 squares or more is available
	for(i = 0; i < 4; i++){
		points[i] += 2*AdjSquare[i]; //adjacent grey squares counts as 2
		points[i] += Distance[i];	//white squares in each row/col are added to points to give more 
	}
	points[0] += Quad[0] + Quad[1];
	points[1] += Quad[0] + Quad[3];
	points[2] += Quad[2] + Quad[3];
	points[3] += Quad[1] + Quad[2];
	
	//if its negative make sure ith direction is not eliminated
	AvailCount = 0; //reuse variable, variable = amount of minimum values
	int minIndex[4] = {10,10,10,10}, minValue = 127, saveI = 10, k = 0; //minIndex = stoes index with the lowest same value (10 = null)
	for(i = 0; i < 4; i++){
		if(points[i] == minValue){
			k++;
			minIndex[k] = i;
			AvailCount++;
		}else if((points[i] < minValue) && (points[i] >= 0)){
			
			for(j = 0; j < 4; j++){
				minIndex[j] = 10;
			}
			k = 0;
			minValue = points[i];
			minIndex[k] = i;
			AvailCount = 1;
			saveI = i;
		} 
	}		
	

	if(AvailCount > 1){
		//randomize the value a of the specific indexes, example: if(index 0 and 3 are the smallest) then randomize whether cat should go 0 or 3
		k = Random()%AvailCount;
		return minIndex[k];
	}	
	else{
		return saveI;
	}
} 

uint8_t GetCatRow (void){
	int i, j;
	for(i = 0; i < 9; i++){//row
		for(j = 0; j < 9; j++){//column
			if(Board[i][j] == cat)
				return i;
		}	
	}
	return 0xFF;
}

uint8_t GetCatCol (void){
	int i, j;
	for(i = 0; i < 9; i++){//row
		for(j = 0; j < 9; j++){//column
			if(Board[i][j] == cat)
				return j;
		}	
	}
	return 0xFF;
}

void moveCat(uint8_t dir){
	
	
	uint8_t catC = GetCatCol();
	uint8_t catR = GetCatRow();
	
	Board_Change(Board[0], catR, catC, yellow);
	ST7735_DrawBitmap((23 + (13*catC)), (16 + (13*catR)), YellowSquare, 10, 10);
	
	
	if (dir == N){
		Board_Change(Board[0], catR -1, catC , cat);
		//ST7735_FillRect((23 + (13* catC)), (7 + (13* (catR  - 1))), 10, 10, ST7735_RED);
		ST7735_DrawBitmap((21 + (13*catC)), (18 + (13* (catR - 1))), catdown, 14, 14);
	}
	if (dir == E){
		Board_Change(Board[0], catR , catC + 1, cat);
			//ST7735_FillRect(23 + (13* (catC + 1)), (7 + (13* (catR))), 10, 10, ST7735_RED);
		ST7735_DrawBitmap((21 + (13*(catC + 1))), (18 + (13* (catR))), catdown, 14, 14);
	}
	if (dir == S){
		Board_Change(Board[0], catR + 1, catC, cat);
		//ST7735_FillRect((23 + (13* catC)), (7 + (13* (catR+ 1))), 10, 10, ST7735_RED);
		ST7735_DrawBitmap((21 + (13*catC)), (18 + (13* (catR + 1))), catdown, 14, 14);
	}
	if (dir == W){
		Board_Change(Board[0], catR , catC -1, cat);
		//ST7735_FillRect(23 + (13* (catC - 1)), (7 + (13* catR )), 10, 10, ST7735_RED);
		ST7735_DrawBitmap((21 + (13*(catC - 1))), (18 + (13* (catR))), catdown, 14, 14);
	}
}	

void EndGame (uint8_t result, uint8_t language){//score = how many times pressed, result = win or loss
	while((GPIO_PORTE_DATA_R & 0x3) > 0){
	}
	EnableInterrupts();
	musicSelect = outcome;
		//Sound_Off();
	Sound_Init();
	Sound_Start(11000);
	ST7735_FillScreen(0x0000);
	if(language == 1){//spanish
		if(result == T){
//			musicSelect = T;
//			Sound_Off();
//			Sound_Start(11000);
			ST7735_DrawBitmap(40, 40, SpanishTrapped, 100, 35);//w=90 h=31
			ST7735_SetCursor(9,6);
			ST7735_OutString("Puntos: ");
			//ST7735_OutChar((0x30 + scoreT));
			LCD_OutDec(score);
		}
		else{
//			musicSelect = L;
//			Sound_Off();
//			Sound_Start(11000);
			ST7735_DrawBitmap(20, 40, SpanishEscaped, 120, 20);//w=120 h=20
		}
			
			ST7735_DrawString(0, 10, "Presione Cualquier", ST7735_YELLOW);
			ST7735_DrawString(0, 11, "Bot\xA2n para", ST7735_YELLOW);
			ST7735_DrawString(0, 12, "Restablecer", ST7735_YELLOW);
			//display reset
			//get gpio port e data
			while((GPIO_PORTE_DATA_R & 0x3) == 0){
			}
			//reset the game
	}
	else{//english
		if(result == T){
//			musicSelect = T;
//			Sound_Off();
//			Sound_Start(11000);
			ST7735_DrawBitmap(40, 40, EnglishTrapped, 100, 35);//w=90 h=30
			ST7735_SetCursor(9,6);
			ST7735_OutString("Points: ");
			LCD_OutDec(score);
		}
		else{
//			musicSelect = L;
//			Sound_Off();
//			Sound_Start(11000);
			ST7735_DrawBitmap(20, 40, EnglishEscaped, 120, 18);//w=120 h=20
		}
			ST7735_DrawString(0, 10, "Press any", ST7735_YELLOW);
			ST7735_DrawString(0, 11, "Button to", ST7735_YELLOW);
			ST7735_DrawString(0, 12, "Reset", ST7735_YELLOW);
			//display reset
			//get gpio port e data
			while((GPIO_PORTE_DATA_R & 0x3) == 0){
			}
			//reset the game
	}
	
}

