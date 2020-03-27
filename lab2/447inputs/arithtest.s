.text
mov r2, #1024
add r3, r2, r2
orr r4, r3, r2
mov r5, r4, LSL #16
sub r6, r4, r2
eor r7, r4, r3
mvn r8, #65
mov r9, r8, LSR #5
mov r10, r8, ASR #5
mov r11, r8, ROR #6
and r12, r8, r2
mul r13, r2, r8
swi #10
