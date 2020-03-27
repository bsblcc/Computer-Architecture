L0:
mov r5, #0x1                    00
b L1                            04
mov r5, #0x10                   08
swi #10                         0c

L1:
cmp r5, r5                      10
bne L3                          14
mov r6, #0x1                    18

L2:
cmp r6, r6                      1c
beq L4                          20
mov r6, #0x10                   24
swi #10                         28

L3:
mov r7, #0x10                   2c
swi #10                         30

L4:
mov r7, #0x1                    34
swi #10                         38


