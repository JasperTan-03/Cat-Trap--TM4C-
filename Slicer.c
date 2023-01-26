//Slicer function to put a sprite onto background

#include <stdint.h>
#include "../inc/tm4c123gh6pm.h"
#include "ST7735.h"
#include "Print.h"
#include "Random.h"
#include "TExaS.h"
#include "ADC.h"
#include "Sound.h"
#include "Timer1.h"



//static int32_t BG [160][128];

//getBg
void getBg(void){
	for (int i = 0; i < 159; i++){
		for (int j = 0; j < 127; j++){
			//BG[i][j] = 0x0000;
		}
	}
	
}

unsigned short tile [160];

void makeBlack(unsigned short *slice, uint16_t w, uint16_t h){
	for (int i = 0; i < w*h; i++){
		if (slice[i] != ST7735_WHITE){
				slice[i] = ST7735_YELLOW; 
			}
		if (slice[i] == ST7735_WHITE){
			slice[i] = ST7735_BLACK;
			}
		}
	}

/*void merge(const uint16_t *slice){
	for (int i = 0; i < 159; i++){
		for (int j = 0; j < 127; j++){
			if (tile[i][j] != ST7735_WHITE){
				slice[i][j] = tile[i][j];
			}
		}
	}
}
*/
