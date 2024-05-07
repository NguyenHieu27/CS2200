!============================================================
! CS 2200 Homework 2 Part 1: mod
!
! Edit any part of this file necessary to implement the
! mod operation. Store your result in $v0.
!============================================================

mod:
    addi    $a0, $zero, 28      ! $a0 = 28, the number a to compute mod(a,b)
    addi    $a1, $zero, 13      ! $a1 = 13, the number b to compute mod(a,b)
    addi    $t0, $zero, 0       ! Initialize $t0 to 0 (x in the C code)

    add     $t0, $a0, $zero     ! assign a to x
    nand    $t1, $a1, $a1
    addi    $t1, $t1, 1         ! inverse b to -b
loop:
    bgt     $t0, $a1, exit      ! Branch to exit if x > b  
    beq     $t0, $a1, exit      ! Branch to exit if x = b
    addi    $t0, $t0, $t1       ! x = x - b
    beq     $zero, $zero, loop  ! Jump back to loop
exit:
    add     $v0, $t0, $zero     ! return x in $v0
    halt                        ! Halt the program
