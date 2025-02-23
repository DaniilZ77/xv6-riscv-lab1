.data
#define BUF_LEN 50
msg_err1: .asciz "Read failure: must contain digits\n"
msg_err2: .asciz "Read failure: must contain one space\n"
msg_err3: .asciz "Read failure: buffer overflow\n"
fmt_str: .asciz "%s\n"
fmt_num: .asciz "%d\n"
debug_str: .asciz "|%d %d|\n"
buffer: .space BUF_LEN
.text
.global main
main:
    la a0, buffer
    call gets
    call atoi
    la s1, buffer
    li s2, ' '
    mv s3, a0
    li t1, '0'
    li t2, '9'
    li t3, '\n'
    la t4, buffer
    li t5, 0
    addi t4, t4, BUF_LEN
read:
    lbu s0, (s1)
    beqz s0, exit
    addi s1, s1, 1
    bne s0, s2, a
    mv t6, s1
    addi t5, t5, 1
    j b
a:
    beq s0, t3, exit
    blt s0, t1, err1
    bgt s0, t2, err1
b:
    addi s4, s1, 1
    beq s4, t4, err3
    j read
exit:
    mv a0, t6
    li t6, 1
    bne t5, t6, err2
    call atoi
    mv s2, a0
    la a0, debug_str
    mv a1, s3
    mv a2, s2
    call printf
    add s3, s3, s2
    mv a1, s3
    la a0, fmt_num
    call printf
    li a7, 2
    li a0, 0
    ecall
err1:
    la a0, msg_err1
    call printf
    j err_exit
err2:
    la a0, msg_err2
    call printf
    j err_exit
err3:
    la a0, msg_err3
    call printf
err_exit:
    li a7, 2
    li a0, 1
    ecall