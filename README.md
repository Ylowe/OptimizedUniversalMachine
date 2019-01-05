# Optimized Universal Machine
Optimized version of the Universal Machine found in the UniversalMachine repo

Use Kcachegrind to optimize the performance of the univeral machine emulator.
Stage 0 is a clone of the original universal machine. Each subsequent iteration is an improvement on the UM C code
See labnotes for details and changes between stages

************* Routine that takes up the most time in the UM *************

Fetching an instruction from the pc and extracting its opcode takes the most
time. Looking at the assembly code, it appears that the program copies the
pc to a new register, increments the copy in the old register, and then uses
the unchanged copy to fetch the instruction:

mov %r15d, %eax              // move pc to new reg
add $0x1, %r15d              // add 1 to original copy of pc
mov 0x4(%rbx, %rax, 4), %eax // fetch instruction from seg0 with new copy of pc

This code could be improved by keeping the pc in one register, fetching
the instruction, and then incrementing the register:

mov 0x4(%rbx, %r15d, 4), %eax // fetch instruction from seg0 at pc
add $0x1, %r15d				  // increment pc

Extracting the opcode from the instruction is done with a simple right-shift,
and this cannot be improved. Likewise, comparing the opcode with that for 
the load value instruction cannot be optimized further.

mov %eax, %ecx    // copy instruction to new reg
shr $0x1c, %exc   // extract opcode from instruction
cmp $0xd, %ecx    // compare opcode with that for load value instruction
je  <main+0x181>  // jump to code for load value if equal

************************************************************************

time spent analyzing problems: 1
time spent solving problems 9
Collaboration: Benny Roover
