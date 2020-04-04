.text
00 add $10, $8, $8             
04 add $11, $8, $9
08 add $12, $9, $8
0c add $13, $9, $9
10 addiu $2, $0, 10
14 syscall

// 60

PIPELINE:
DCODE: OP (PC=0040001c inst=00000000) src1=R-1 (00000000) src2=R-1 (00000000) dst=R-1 valid 0 (00000000) br=0 taken=0 dest=00000000 mem=0 addr=00000000
EXEC : OP (PC=00400018 inst=00000000) src1=R0 (00000000) src2=R0 (00000000) dst=R0 valid 0 (00000000) br=0 taken=0 dest=00000000 mem=0 addr=00000000
MEM  : OP (PC=00400014 inst=0000000c) src1=R2 (0000000a) src2=R3 (00000000) dst=R0 valid 1 (00000000) br=0 taken=0 dest=00000000 mem=0 addr=00000000
WB   : OP (PC=00400010 inst=2402000a) src1=R0 (00000000) src2=R-1 (00000000) dst=R2 valid 1 (0000000a) br=0 taken=0 dest=00000000 mem=0 addr=00000000

R2 = 0000000a
MIPS-SIM> run 1

Simulating for 1 cycles...



----

PIPELINE:
DCODE: (null)
EXEC : OP (PC=0040001c inst=00000000) src1=R0 (00000000) src2=R0 (00000000) dst=R0 valid 0 (00000000) br=0 taken=0 dest=00000000 mem=0 addr=00000000
MEM  : OP (PC=00400018 inst=00000000) src1=R0 (00000000) src2=R0 (00000000) dst=R0 valid 1 (00000000) br=0 taken=0 dest=00000000 mem=0 addr=00000000
WB   : OP (PC=00400014 inst=0000000c) src1=R2 (0000000a) src2=R3 (00000000) dst=R0 valid 1 (00000000) br=0 taken=0 dest=00000000 mem=0 addr=00000000
