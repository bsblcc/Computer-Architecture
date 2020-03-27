mov r2, #0
mov r3, #1
mvn r4, #1

mov r5, #0x0

b L1

L0:
swi #10

L1:
add r5, r5, #0x10
b L2
swi #10

L2:
add r5, r5, #0x100
cmp r3, r4
bne L3
swi #10

L3:
add r5, r5, #0x1000
tst r0, r0
beq L4
swi #10

L4:
cmp r3, #0
ble bad
add r5, r5, #0x10000

L5:
add r5, r5, #0x100000
cmp r3, #0
bgt L6
swi #10

L6:
add r5, r5, #0x1000000
swi #10

bad:
swi #10

