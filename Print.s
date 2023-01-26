; Print.s
; Student names: Rohan Jain, Jasper Tan
; Last modification date: 3/28/22
; Runs on TM4C123
; EE319K lab 7 device driver for any LCD
;
; As part of Lab 7, students need to implement these LCD_OutDec and LCD_OutFix
; This driver assumes two low-level LCD functions
; ST7735_OutChar   outputs a single 8-bit ASCII character
; ST7735_OutString outputs a null-terminated string 

	PRESERVE8
    IMPORT   ST7735_OutChar
    IMPORT   ST7735_OutString
    EXPORT   LCD_OutDec
    EXPORT   LCD_OutFix

    AREA    |.text|, CODE, READONLY, ALIGN=2
    THUMB



;-----------------------LCD_OutDec-----------------------
; Output a 32-bit number in unsigned decimal format
; Input: R0 (call by value) 32-bit unsigned number
; Output: none
; Invariables: This function must not permanently modify registers R4 to R11
; R0=0,    then output "0"
; R0=3,    then output "3"
; R0=89,   then output "89"
; R0=123,  then output "123"
; R0=9999, then output "9999"
; R0=4294967295, then output "4294967295"

FP RN R11 

	
LCD_OutDec
count EQU 0
num EQU 4	;binding
	push {R4- R9, R11, LR}
	
	CMP R0, #0
	BNE allocation
	ADD R0, #0x30
	BL ST7735_OutChar
	pop {R4- R9, R11, LR}
	BX LR
	
allocation
	
	SUB SP, #8 ;	ALLOCATION
	MOV FP, SP
	MOV R4, #0 ;for count
	STR R4, [FP, #count] ;store 0 into count
	STR R0, [FP, #num] ;store input into num
	MOV R5, #10 ;to be used for division
loop
	LDR R4, [FP, #num]
	CMP R4, #0 ;checking if num = 0
	BEQ next
	UDIV R6, R4, R5 ; R6= num / 10 , ex: 72 = 725/10 
	MUL R7, R6, R5 ; R7 = 10*R6 , ex: 720 = 72*10
	SUB R8, R4, R7 ;R8 = num - R7 = remainder , ex: 5 = 725 - 720
	push {R8}
	STR R6, [FP, #num] ;store new num back onto num in stack ex: 725 --> 72
	
	LDR R6, [FP, #count]
	ADD R6, #1 ; count++
	STR R6, [FP, #count]
	B loop

next
	LDR R4, [FP, #count]
	CMP R4, #0
	BEQ done
	pop {R0}
	ADD R0, #0x30 ;ASCII offset
	BL ST7735_OutChar
	SUB R4, #1 ;	count--
	STR R4, [FP, #count]
	B next

done
	ADD SP, #8 ; DEALLOCATION
	pop {R4- R9, R11, LR}
	
      BX  LR
;* * * * * * * * End of LCD_OutDec * * * * * * * *

; -----------------------LCD _OutFix----------------------
; Output characters to LCD display in fixed-point format
; unsigned decimal, resolution 0.001, range 0.000 to 9.999
; Inputs:  R0 is an unsigned 32-bit number
; Outputs: none
; E.g., R0=0,    then output "0.000"
;       R0=3,    then output "0.003"
;       R0=89,   then output "0.089"
;       R0=123,  then output "0.123"
;       R0=9999, then output "9.999"
;       R0>9999, then output "*.***"
; Invariables: This function must not permanently modify registers R4 to R11
LCD_OutFix
	push {R4- R9, R11, LR}
	
	MOV R4, #9999
	CMP R0, R4
	BLS continue

	MOV R0, #0x2A
	BL ST7735_OutChar
	MOV R0, #0x2E
	BL ST7735_OutChar
	MOV R0, #0x2A
	BL ST7735_OutChar
	MOV R0, #0x2A
	BL ST7735_OutChar
	MOV R0, #0x2A
	BL ST7735_OutChar
	pop {R4- R9, R11, LR}
	BX LR

continue
	;rest of outfix here
countFIX EQU 0
numFIX EQU 4
    SUB SP, #8 ;    ALLOCATION
    MOV FP, SP
    MOV R4, #0 ;for count
    STR R4, [FP, #count] ;store 0 into count
    STR R0, [FP, #num] ;store input into num
    MOV R5, #10 ;to be used for division
loop2
    LDR R4, [FP, #num]
    LDR R9, [FP, #count]

    CMP R9, #3
    BEQ next2
    CMP R4, #0
    BEQ next2
    UDIV R6, R4, R5 ; R6= num / 10 , ex: 72 = 725/10 
    MUL R7, R6, R5 ; R7 = 10R6 , ex: 720 = 7210
    SUB R8, R4, R7 ;R8 = num - R7 = remainder , ex: 5 = 725 - 720
    push {R8}
    STR R6, [FP, #num] ;store new num back onto num in stack ex: 725 --> 72

    LDR R6, [FP, #count]
    ADD R6, #1 ; count++
    STR R6, [FP, #count]
    B loop2

next2
loop0
    LDR R9, [FP, #count]
    CMP R9, #3
    BEQ MOVEON
    MOV R10, #0
    push{r10}
    ADD R9, #1
    STR R9, [FP, #count]
    B loop0


MOVEON
    MOV R10, #0x2E
    PUSH{R10}
    LDR R6, [FP, #count]
    ADD R6, #1 ; count++
    STR R6, [FP, #count]

    LDR R4, [FP, #num]
    LDR R9, [FP, #count]

    UDIV R6, R4, R5 ; R6= num / 10 , ex: 72 = 725/10 
    MUL R7, R6, R5 ; R7 = 10R6 , ex: 720 = 7210
    SUB R8, R4, R7 ;R8 = num - R7 = remainder , ex: 5 = 725 - 720
    push {R8}
    STR R6, [FP, #num] ;store new num back onto num in stack ex: 725 --> 72

    LDR R6, [FP, #count]
    ADD R6, #1 ; count++
    STR R6, [FP, #count]


REPRINT
    LDR R4, [FP, #count]
    CMP R4, #0
    BEQ done2
    pop {R0}
    CMP R0, #0x2E
    BEQ SKIP
    ADD R0, #0x30 ;ASCII offset to convert int to char
SKIP
    BL ST7735_OutChar
    SUB R4, #1 ;    count--
    STR R4, [FP, #count]
    B REPRINT

done2
    ADD SP, #8 ; DEALLOCATION
    pop {R4- R9, R11, LR}
    BX  LR
 
;* * * * * * * * End of LCD_OutFix * * * * * * * *

     ALIGN                           ; make sure the end of this section is aligned
     END                             ; end of file
