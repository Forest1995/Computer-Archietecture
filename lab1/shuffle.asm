	.ORIG x3000
	LEA R0, START			
	LDW R1, R0, #0           ;LOAD ADDRESS OF START OF DATA   R1=0x4000
	LEA R0, MASK
	LDW R2, R0, #0           ;LOAD ADDRESS OF MASK  R2=0x4004
	LDB R3, R2, #0           ;R3=The content in address 0x4004
	AND R4, R4, #0		     ;R4 WILL BE COUNTER    R4=0


LOOP ADD R4, R4, #1
         AND R0, R3, #3	         ;CLEAR ALL BUT BITS 1,0 OF MASK
	BRz FIRST
	ADD R0, R0, #-1
	BRz SECOND
	ADD R0, R0, #-1
	BRz THIRD
	BRnzp FOURTH


FIRST	LDB R5, R1, #0      ;R5=the content in address 0x4000
	ADD R7, R2, R4
	STB R5, R7, #0          ;The content in R5 in stored in address (R2+R4=0x4004+R4)
	RSHFL R3, R3, #2
	BRnzp CHECK

SECOND	LDB R5, R1, #1      ;R5=the content in address 0x4001
	ADD R7, R2, R4
	STB R5, R7, #0          ;The content in R5 in stored in address (R2+R4=0x4004+R4)
	RSHFL R3, R3, #2
	BRnzp CHECK

THIRD	LDB R5, R1, #2      ;R5=the content in address 0x4002
    ADD R7, R2, R4
	STB R5, R7, #0          ;The content in R5 in stored in address (R2+R4=0x4004+R4)
	RSHFL R3, R3, #2
	BRnzp CHECK

FOURTH	LDB R5, R1, #3      ;R5=the content in address 0x4003
	ADD R7, R2, R4
	STB R5, R7, #0          ;The content in R5 in stored in address (R2+R4=0x4004+R4)
	RSHFL R3, R3, #2
	BRnzp CHECK

CHECK	 ADD R6, R4, #-4
BRn      LOOP

         HALT
START	.FILL x4000
MASK	.FILL x4004
	.END
