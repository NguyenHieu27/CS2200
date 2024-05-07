! Spring 2024 Revisions by Karan Patel

! This program executes pow as a test program using the LC 1001 calling convention
! Check your registers ($v0) and memory to see if it is consistent with this program

main:	lea $sp, initsp                         ! initialize the stack pointer
        lw $sp, 0($sp)                          ! finish initialization

        lea $a0, BASE                           ! load base for pow
        lw $a0, 0($a0)
        lea $a1, EXP                            ! load power for pow
        lw $a1, 0($a1)
        lea $at, POW                            ! load address of pow

        addi $t0, $zero, 2200                   ! set $t0 for nand check
        fabs $t0
        nand $t0, $t0, $t0

        
        add $t0, $zero, $zero
        add $t1, $zero, $zero
	add $t2, $zero, $zero

        addi $t0, $t0, -1                       ! set $t0 for fabs check
        addi $t1, $t1, 1                        ! set $t1 for beq check
        fabs $t0
        bgt $t0, $zero, CHECK                   ! halt if negative
        halt
CHECK:
        beq  $t0, $t1, CONTINUE			        ! test beq
        halt

CONTINUE:
        ! RL RR
        addi $t0, $zero, 15                     ! set $t0 for RL check
        addi $t1, $zero, 4                      ! set $t1 for RR check
        ror $t2, $t0, $t1
        rol $t2, $t2, $t1
        beq $t2, $t0, CALL                          ! test RL RR
        halt

CALL:

        jalr $at, $ra                           ! run pow
        lea $a0, ANS                            ! load base for pow
        sw $v0, 0($a0)

        halt                                    ! stop the program here
        addi $v0, $zero, -1                     ! load a bad value on failure to halt

BASE:   .fill 2
EXP:    .fill 8
ANS:	.fill 0                                 ! should come out to 256 (BASE^EXP)

POW:    addi $sp, $sp, -1                       ! allocate space for old frame pointer
        sw $fp, 0($sp)

        addi $fp, $sp, 0                        ! set new frame pointer

        bgt $a1, $zero, BASECHK                 ! check if $a1 is zero
        beq $zero, $zero, RET1                  ! if the exponent is 0, return 1
        
BASECHK:
        bgt $a0, $zero, WORK                    ! if the base is 0, return 0
        beq $zero, $zero, RET0                                 

WORK:
        addi $a1, $a1, -1                       ! decrement the power

        lea $at, POW                            ! load the address of POW
        addi $sp, $sp, -2                       ! push 2 slots onto the stack
        sw $ra, -1($fp)                         ! save RA to stack
        sw $a0, -2($fp)                         ! save arg 0 to stack
        jalr $at, $ra                           ! recursively call POW
        add $a1, $v0, $zero                     ! store return value in arg 1
        lw $a0, -2($fp)                         ! load the base into arg 0
        lea $at, MULT                           ! load the address of MULT
        jalr $at, $ra                           ! multiply arg 0 (base) and arg 1 (running product)
        lw $ra, -1($fp)                         ! load RA from the stack
        addi $sp, $sp, 2

        beq $zero, $zero, FIN                   ! unconditional branch to FIN

RET1:   add $v0, $zero, $zero                   ! return a value of 0
	addi $v0, $v0, 1                        ! increment and return 1
        beq $zero, $zero, FIN                   ! unconditional branch to FIN

RET0:   add $v0, $zero, $zero                   ! return a value of 0

FIN:	lw $fp, 0($fp)                          ! restore old frame pointer
        addi $sp, $sp, 1                        ! pop off the stack
        jalr $ra, $zero

MULT:   add $v0, $zero, $zero                   ! return value = 0
        addi $t0, $zero, 0                      ! sentinel = 0

        
        addi $s0, $zero, 1                      ! sll and srl check
        addi $s1, $zero, 1
        addi $s2, $zero, 2
        sll $s0, $s0, $s1
        beq $s0, $s2, SRL_CHECK
        halt

SRL_CHECK:
        addi $s0, $zero, 2
        addi $s1, $zero, 1
        
        srl $s0, $s0, $s1
        beq $s0, $s1, AGAIN
        halt



AGAIN:  add $v0, $v0, $a0                       ! return value += argument0
        addi $t0, $t0, 1                        ! increment sentinel
        bgt $a1, $t0, AGAIN                     ! while argument > sentinel, loop again
        jalr $ra, $zero                         ! return from mult

initsp: .fill 0xA000
