//Board.h
#include <stdint.h>
#include "../inc/tm4c123gh6pm.h"

void Board_Init(uint8_t *pointer, uint8_t rows, uint8_t columns);

void Board_Change(uint8_t *pointer, uint8_t row, uint8_t column, uint8_t color);
