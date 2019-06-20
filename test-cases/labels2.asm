    la $t9, some_label
    add $s0, $s1, $s2
    lui $t0, 345
syscall
lbu $t0,-345($t1)

some_label:
jr $ra
bne $s0, $s1, some_label
nop
nop
beq $t1, $t0, some_label
beq $t1, $zero, next
next:
j next
jal some_label
ori $t1, $t2, 500
slti $s0, $s1, -500
sltiu $s0, $s1, 600
sll $t8, $t9, 4
srl $t8, $t9, 3