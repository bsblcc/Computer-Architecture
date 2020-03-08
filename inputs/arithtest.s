00  mov r2, #1024
04  add r3, r2, r2
08  orr r4, r3, r2
0c  mov r5, r4, LSL #16
10  sub r6, r4, r2
14  eor r7, r4, r3
18  mvn r8, #65
1c  mov r9, r8, LSR #5
20  ov r10, r8, ASR #5
24  mov r11, r8, ROR #6
28  and r12, r8, r2
2c  mul r13, r2, r8
30  swi #10


r2 = 1024
r3 = 2048
r4 = 3072
r5 = ‭201326592‬  0xc00 0000
r6 = 2048
r7 = 1024
r8 = -66
r9 = 0x 7 ff ff fd
r10 = 0xffff fffd
r11 = 0xfbff fffe
r12 = 1024
r13 = 0xfffef800