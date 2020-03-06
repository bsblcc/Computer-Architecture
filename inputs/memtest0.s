.text

mov r0, #0x10000000

mov r1, #0xff
add r2, r1, r1
add r3, r2, r2
add r4, r3, #0x30000

str r1, [r0]
str r2, [r0, #4]
str r3, [r0, #8]
str r4, [r0, #12]

ldr r5, [r0]
ldr r6, [r0, #4]
ldr r7, [r0, #8]
ldr r8, [r0, #12]

mov r1, #0xbe

strb r5, [r0, #1]
strb r1, [r0, #2]
strb r5, [r0, #7]
strb r1, [r0, #9]

ldr r9, [r0]
ldr r10, [r0, #4]
ldr r11, [r0, #8]
ldrb r12, [r0, #9]

mov r13, r5
add r13, r13, r6
add r13, r13, r7
add r13, r13, r8
add r13, r13, r9
add r13, r13, r10
add r13, r13, r11
add r13, r13, r12

swi #10



