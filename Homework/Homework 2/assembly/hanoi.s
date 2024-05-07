!============================================================
! CS 2200 Homework 2 Part 2: Tower of Hanoi
!
! Apart from initializing the stack,
! please do not edit mains functionality. You do not need to
! save the return address before jumping to hanoi in
! main.
!============================================================

main:
                                    ! TODO: Here, you need to get the address of the stack
                                    ! using the provided label to initialize the stack pointer.

    lea     $sp, stack              ! Initialize stack pointer
    lw      $fp, 0($sp)             ! Load label address into $sp and store into $fp
    lw      $sp, 0($sp)             ! Use $sp as base register to load the value (0xFFFF) into $sp.


    lea     $at, hanoi              ! loads address of hanoi label into $at

    lea     $a0, testNumDisks3      ! loads address of number into $a0
    lw      $a0, 0($a0)             ! loads value of number into $a0

    jalr    $at, $ra                ! jump to hanoi, set $ra to return addr
    halt                            ! when we return, just halt

hanoi:
                                    ! TODO: perform post-call portion of the calling convention.
                                    ! Make sure to save any registers you will be using!

    addi $sp, $sp, -1               
    sw $fp, 0($sp)                  ! Save previous fp into stack
    add $fp, $sp, $zero             ! Set new fp to current sp

                                    ! TODO: Implement the following pseudocode in assembly:

    addi    $t0, $zero, 1           ! Set up $t0 = 1 for condition check
    beq     $a0, $t0, base          ! IF ($a0 == 1) -> GOTO base
    beq     $zero, $zero, else      ! ELSE -> GOTO else

else:
                                    ! TODO: perform recursion after decrementing the parameter by 1.
                                    ! Remember, $a0 holds the parameter value.

    addi    $a0, $a0, -1            ! Decrement parameter by 1
    addi    $sp, $sp, -1
    sw      $ra, 0($sp)
    lea     $at, hanoi
    jalr    $at, $ra                ! Jump to 'hanoi' subrountine

                                    ! TODO: Implement the following pseudocode in assembly:

    lw      $ra, -1($sp)            ! Restore return address to $ra
    addi    $sp, $sp, 1             
    add     $v0, $v0, $v0           ! $v0 = 2 * $v0
    addi    $v0, $v0, 1             ! $v0 = 2 * $v0 + 1
    beq     $zero, $zero, teardown  ! Return to caller (RETURN $v0)

base:
                                    ! TODO: Return 1

    addi    $v0, $zero, 1           ! Return $v0 = 1
    beq     $zero, $zero, teardown  ! Return to caller

teardown:
                                    ! TODO: perform pre-return portion of the calling convention

    lw $fp, 0($fp)                  ! Restore previous fp to $fp
    addi $sp, $fp, 0                ! Set sp back to the same position as fp
    jalr    $ra, $zero              ! Return to caller



stack: .word 0xFFFF                 ! the stack begins here


! Words for testing \/

! 1
testNumDisks1:
    .word 0x0001

! 10
testNumDisks2:
    .word 0x000a

! 20
testNumDisks3:
    .word 0x0014
