L0:
b L1            00

L1:
cmp r0, r0      04
bne L3          08

L2:
cmp r0, r0      0c
beq L4          10

L3:
mov r0, #0xbb   14
swi #10         18

L4:
mov r0, #0xdd   1c
swi #10         20
