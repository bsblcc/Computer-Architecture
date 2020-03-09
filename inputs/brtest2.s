00  mov r2, #0      
04  mov r3, #1       
08  mvn r4, #1

0c  mov r5, #0x0

10  b L1

L0:
14  swi #10

L1:
18  add r5, r5, #0x10
1c  b L2
20  swi #10

L2:
24  add r5, r5, #0x100
28  cmp r3, r4
2c  bne L3
30  swi #10

L3:
34  add r5, r5, #0x1000
38  tst r0, r0
3c  beq L4
40  swi #10

L4:
44  cmp r3, #0
48  ble bad
4c  add r5, r5, #0x10000

L5:
50  add r5, r5, #0x100000
54  cmp r3, #0
58  bgt L6
5c  swi #10

L6:
60  add r5, r5, #0x1000000
swi #10

bad:
swi #10

