L0:
b L1

L1:
cmp r0, r0
bne L3

L2:
cmp r0, r0
beq L4

L3:
mov r0, #0xbb
swi #10

L4:
mov r0, #0xdd
swi #10
