0x400000    mov r1, #0xff
      04    add r2, r1, #0xff00
      08    mov r3, #0xff
      0c    add r3, r3, r3, LSL #8
      10    cmp r2, r3
      14    bne bad
      18    mvn r8, #0x8000
      1c    add r8, r8, #1
      20    mov r9, #0xff
      24    add r9, r9, LSL #8
      28    mov r9, r9, LSL #16
      2c    orr r9, r9, #0x8000
      30    cmp r8, r9
      34    bne bad
      38    b good
            good:
      3c    mov r0, #0xdd
      40    swi #10

            bad:
      44    mov r0, #0xbb
      48    swi #10
