.text
mov r1, #0x12000000
add r1, r1, #0x340000
add r1, r1, #0x5600
add r1, r1, #0x78
mov r2, #0x12                               10
mov r2, r2, LSL #24
orr r2, r2, #0x340000
orr r2, r2, #0x5600
orr r2, r2, #0x78                           20
cmp r1, r2  
bne bad
and r3, r1, #0xff000000
cmp r3, #0x12000000                         30
bne bad
and r4, r1, #0xff00                         38
cmp r4, #0x5600
bne bad                                     40
and r5, r1, #0xf000000f
cmp r5, #0x10000008
bne bad                                     4c
mov r6, #0xff                               50
eor r7, r6, #0xff0
mov r8, #0xf
add r8, r8, #0xf00
cmp r7, r8                                  60
bne bad
b good

good:
mov r0, #0xdd
swi #10

bad:
mov r0, #0xbb
swi #10
