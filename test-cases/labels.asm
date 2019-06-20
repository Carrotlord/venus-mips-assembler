# Testing of MIPS commands
# By Jiangcheng Oliver Chu

# Addition
TOP:
add $t0,$t1,$t2
add $s0,$sp,$ra
addu $s0,$sp,$ra

# Bitwise
next:
and $t0,$t1,$t2
or $v0,$v1,$v1
nor $s0,$s1,$s7

# Set less than
bottom:
slt $t8,$t9,$t0
sltu $s0,$s1,$s7

# Subtraction
sub $s0,$s1,$s7
subu $s0,$s1,$s7

# Immediate bitwise
andi $t0, $t1, 1600

# Immediate addition
addi $t0,$t1,500
addiu $t0,$t1,-0x500

# Branching
beq $t0,$t1,600
bne $t0,$t1,-900

beq $t1,$t0,600
bne $t1,$t0,-900

beq $t1,$t0,next
bne $t1,$t0,TOP
