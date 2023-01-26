//Board.c
#include <stdint.h>
#include "../inc/tm4c123gh6pm.h"
#include "ST7735.h"

#define yellow 0
#define grey 1
#define cat  2
#define selected 3
#define border 4

void Board_Init(uint8_t *pointer, uint8_t rows, uint8_t columns){
	int i, j;
	for(i = 0; i < rows; i++){
		for(j = 0; j < columns; j++){
			*(pointer + (i*(rows)) + j) = yellow;
		}
	}	
	*(pointer + 40) = cat;
}

void Board_Change(uint8_t *pointer, uint8_t row, uint8_t column, uint8_t color){
	*(pointer + (9*(row)) + (column)) = color;
}

void movingSquare(uint8_t *pointer, uint8_t row, uint8_t column){
		
}
