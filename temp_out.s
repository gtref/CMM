.section .rdata
fmt_int:
    .asciz "%d\n"
.text
.globl main
main:
    pushq %rbp
    movq %rsp, %rbp
    subq $64, %rsp
    movq $0, %rax
    movq %rax, -8(%rbp)
start:
    movq $1, %rax
    pushq %rax
    movq -8(%rbp), %rax
    popq %rbx
    addq %rbx, %rax
    movq %rax, -8(%rbp)
    movq -8(%rbp), %rax
    movq %rax, %rdx
    leaq fmt_int(%rip), %rcx
    call printf
    movq $6, %rax
    pushq %rax
    movq -8(%rbp), %rax
    popq %rbx
    cmpq %rbx, %rax
    setl %al
    movzbq %al, %rax
    cmpq $0, %rax
    je .L_else_0
    jmp start
    jmp .L_end_1
.L_else_0:
.L_end_1:
    movq $0, %rax
    movq %rbp, %rsp
    popq %rbp
    ret
    movq %rbp, %rsp
    popq %rbp
    ret
