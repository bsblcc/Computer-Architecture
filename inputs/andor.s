.text
mov r1, #0x12000000
add r1, r1, #0x340000
add r1, r1, #0x5600
add r1, r1, #0x78
mov r2, #0x12
mov r2, r2, LSL #24
orr r2, r2, #0x340000
orr r2, r2, #0x5600
orr r2, r2, #0x78
cmp r1, r2
bne bad
and r3, r1, #0xff000000
cmp r3, #0x12000000
bne bad
and r4, r1, #0xff00
cmp r4, #0x5600
bne bad
and r5, r1, #0xf000000f
cmp r5, #0x10000008
bne bad
mov r6, #0xff
eor r7, r6, #0xff0
mov r8, #0xf
add r8, r8, #0xf00
cmp r7, r8
bne bad
b good

good:
mov r0, #0xdd
swi #10

bad:
mov r0, #0xbb
swi #10
