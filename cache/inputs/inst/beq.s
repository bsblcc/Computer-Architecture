

------------------------------cache line #0-------------------------------------
.text
00  beq $8, $9, nottaken
04  beq $zero, $zero, taken1
08  addiu $2, $0, 10
0c  syscall

nottaken:
10  addiu $2, $0, 10
14  syscall

taken1:
18  beq $8, $11, taken2
1c  addiu $2, $0, 10


------------------------------cache line #1-------------------------------------


20  syscall

taken2: 
24  addiu $2, $0, 10            this instruction doesn't execute?? --> fetched wrong instruction word
28  syscall

