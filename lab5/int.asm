.ORIG x1200
ADD R6, R6, #-2
STW R0, R6, #0
ADD R6, R6, #-2
STW R1, R6, #0
ADD R6, R6, #-2
STW R2, R6, #0
ADD R6, R6, #-2
STW R3, R6, #0

LEA R2, BASE
LDW R0, R2, #0
LEA R2, MASK
LDW R1, R2, #0
LEA R2, LENGTH
LDW R2, R2, #0

LOOP LDW R3, R0, #0
AND R3, R3, R1
STW R3, R0, #0
ADD R0, R0, #2
ADD R2, R2, #-1
BRP LOOP

LDW R3, R6, #0
ADD R6, R6, #2
LDW R2, R6, #0
ADD R6, R6, #2
LDW R1, R6, #0
ADD R6, R6, #2
LDW R0, R6, #0
ADD R6, R6, #2

RTI

LENGTH .FILL #128
BASE .FILL x1000
MASK .FILL xFFFE

.END
