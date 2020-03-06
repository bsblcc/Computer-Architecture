mov r1, #0xff
add r2, r1, #0xff00
mov r3, #0xff
add r3, r3, r3, LSL #8
cmp r2, r3
bne bad
mvn r8, #0x8000
add r8, r8, #1
mov r9, #0xff
add r9, r9, LSL #8
mov r9, r9, LSL #16
orr r9, r9, #0x8000
cmp r8, r9
bne bad
b good

good:
mov r0, #0xdd
swi #10

bad:
mov r0, #0xbb
swi #10
