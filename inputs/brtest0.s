L0:
mov r5, #0x1
b L1
mov r5, #0x10
swi #10

L1:
cmp r5, r5
bne L3
mov r6, #0x1

L2:
cmp r6, r6
beq L4
mov r6, #0x10
swi #10

L3:
mov r7, #0x10
swi #10

L4:
mov r7, #0x1
swi #10


