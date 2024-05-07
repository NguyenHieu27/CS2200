The LC-3300 Assembler and Simulator
===============

To aid in testing your processor, we have provided an *assembler* and
a *simulator* for the LC-3300 architecture. The assembler supports
converting text `.s` files into either binary (for the simulator) or
hexadecimal (for pasting into CircuitSim) formats.

Requirements
-----------

The assembler and simulator run on any version of Python 3+. An
instruction set architecture definition file is required along with
the assembler. The LC-3300 assembler definition is included.

Included Files
-----------

* `assembler.py`: the main assembler program
* `lc3300.py`: the LC-3300 assembler definition
* `lc3300-sim.py`: the LC-3300 simulator program

Using the Assembler
-----------

### Assemble and Simulate

Example usage to assemble `test.s`:

    python3 assembler.py -i lc3300 --sym test.s

... the resulting `test.bin` and `test.sym` files can then be loaded
in the simulator (see below).

### Assemble for CircuitSim

To output assembled code in hexadecimal (for use with *CircuitSim*):

    python3 assembler.py -i lc3300 --hex test.s

You can then open the resulting `test.hex` file in your favorite text
editor.  In CircuitSim, right-click on your RAM, select **Edit
Contents...**, and then copy-and-paste the contents of the hex file
into the window.

Do not use the Open or Save buttons in CircuitSim, as it will not
recognize the format.

Using the Simulator
-----------

Example usage to simulate `test.bin` (generated by the assembler):

    python lc3300-sim.py test.bin

Then type `help` at the prompt for a list of available commands:

    (sim) help
    
    Welcome to the simulator text interface. The available commands are:
    
        r[un] or c[ontinue]             resume execution until next breakpoint
        s[tep]                          execute one instruction
        b[reak] <addr or label>         view and set breakpoints
                                        ex) 'break'
                                        ex) 'break 0x3'
                                        ex) 'break main'
        print <lowaddr>-<highaddr>      print the values in memory between <lowaddr> and <highaddr>
                                        ex) 'print 0x20-0x30'
        q[uit]                          exit the simulator

You can use the simulator to explore how your processor is expected to
behave, step through code instruction-by-instruction, and examine the
values of registers and memory at each stage of the program.

Assembler Pseudo-ops
-----------

In addition to the syntax described in the LC-3300 ISA reference,
the assembler supports the following *pseudo-instructions*:

* `.fill`: fills a word of memory with the literal value provided

For example, `mynumber: .fill 0x500` places `0x500` at the memory
location labeled by `mynumber`.

* `.word`: an alias for `.fill`