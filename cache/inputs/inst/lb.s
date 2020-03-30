.text

        # Store word aligned
        sw $11, 0($10)

        # Load byte
        lb $12, 0($10)

        # Load byte
        lb $13, 1($10)

        # Load byte
        lb $14, 2($10)

        # Load byte
        lb $15, 3($10)

	addiu $2, $0, 10
	syscall
0x400000    ad4b0000    sw $11, 0($10)
0x400004    814c0000    lb $12, 0($10)
0x400008    814d0001    lb $13, 1($10)
0x40000c    814e0002    lb $14, 2($10)
0x400010    814f0003    lb $15, 3($10)
0x400014    2402000a    addiu $2, $0, 10
0x400018    0000000c    syscall





i 10 0x10000000
i 11 0x123456ff