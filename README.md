# Holberton War, yet another corewar !
## What is it ?
A *corewar* is a game, where *processes* fight to make their *core* win. They fight using a *virtual machine*, where each processes are executed on the same *shared* memory. To win, each core will have to destroy the other cores processes, by making them execute a given instruction.

## Virtual machine, shared memory, instructions, what ?
Well, yeah. It sounds a bit complicated, but it is actually fairly easy. Let's start by defining the different terms

### Virtual machine

A *virtual machine* is actually an emulator. An emulator is a programs that allows to execute a system into another.

You probably have heard about *VMWare*, which allows to run a system inside another system. *VMWare*, as far as I remember, is limited to the same CPU architecture ( actually limited to x86 from Intel ). But there is also other emulator, like *QEMU*, which allows to run different system with different target CPU. For example, *Google* is using *QEMU* to emulate an ARM CPU on your desktop computer so that you can run Android on your system, which can be useful for *debugging*.

Another example of virtual machine would be *JAVA* or *.NET*, which, following the moto "compile once, run anywhere" ( or as dev says "compile once, debug everywhere" ), compile your code not to a *native* CPU instructions, but to a *virtual* CPU instructions, defined by the creator of these platforms. Then when you launch the program compiled this way, it actually launch a virtual machine that will interpret the *virtual* CPU code to *native* CPU code your system can understand. Which means that you can compile a JAVA code on windows, take the resulting binary, and execute it on Linux, or OSX, or any system that have a JAVA virtual machine.

Yet another example would be a console emulator, like *Dolphin* that allows you to run GameCube or WII game on your computer.

As you can imagine, a virtual machine can be something very **VERY** complicated, so for this project we are going to limit a bit the scope of what you will have to do.

For fun I would recommend watching this :

  https://www.youtube.com/watch?v=y71lli8MS8s

### shared memory

In a normal system, all processes will not have the same memory, for obvious security reason. Each programs that you launch on OSX, or Linux, etc. have their own memory space, which we call *virtual* because, at the end, all processes use the same exact real physical memory


Each core is a binary file that contains a bunch of instructions. These instructions are well defined ( later in this documents )

(1) add
in(register|immediate), in(register|immediate), out(register)

(2) sub
in(register|immediate), in(register|immediate), out(register)

(3) and
in(register|immediate), in(register|immediate), out(register)

(4) or
in(register|immediate), in(register|immediate), out(register)

(5) jnz
in(register|immediate), in(register|immediate), out(register)

(6) ld
in(immediate|address), out(register)

(7) st
in(register), out(address|register)


pc
reg 16
zero flag 1
