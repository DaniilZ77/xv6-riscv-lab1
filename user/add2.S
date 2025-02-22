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
    la a0, buffer
    call atoi
    mv s3, a0
    la s1, buffer
    li s2, ' '
    la s4, buffer
    li t1, 0
    li t4, 2
    li t2, '0'
    li t3, '9'
    li t5, '\n'
    la t6, buffer
    addi t6, t6, BUF_LEN
    j check
check_space:
    addi t1, t1, 1
    beq t1, t4, err2
check:
    lbu s0, (s1)
    beqz s0, end
    addi s1, s1, 1
    beq s0, s2, check_space
    beq s0, t5, end
    beq s1, t6, err3
    blt s0, t2, err1
    bgt s0, t3, err1
    j check
end:
    la s1, buffer
space:
    lbu s0, (s1)
    beqz s0, err2
    addi s1, s1, 1
    bne s0, s2, space
exit:
    mv a0, s1
    call atoi
    mv s0, a0
    mv a2, a0
    la a0, debug_str
    mv a1, s3
    call printf
    add s0, s0, s3
    mv a1, s0
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
    j err_exit
err_exit:
    li a7, 2
    li a0, 1
    ecall