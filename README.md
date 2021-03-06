[![Build status](https://ci.appveyor.com/api/projects/status/motlasbbysk0cl0o?svg=true)](https://ci.appveyor.com/project/olivierchatry/holbertonwar)

[![Build Status](https://travis-ci.org/olivierchatry/HolbertonWar.svg)](https://travis-ci.org/olivierchatry/HolbertonWar)

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

## building project
	1. git clone the repository to your local computer
	2. go to the local repository folder
	3. git submodule init && git submodule update
	4. go to "platforms" folder
	5. depending on your OS :
		* linux 	: ./premake5.linux gmake
		* osx   	: ./premake5.osx gmake
		* windows : premake5.exe vs2015 ( or vs2013 etc. )
		be sure to check that the binary have exec right ( chmod +x )
	6. got to folder "build", from the local repository folder
	7. on linux, osx, execute "make", this should create the binary ( in same folder )
	8. on windows, open HolbertonWar.sln

## instructions set

> **If not said so, all addresses are modulo 512.**

> **Address are 16bits wide**
> **Immediate are 32bits wide**
> **Register are 32bits wide**

### live
```
	in(immediate) core id
```
* **Does not have an argument coding byte**.
* Reset the number of cycles until the process dies, set the live counter for core if to the current cycle.
* Cycles : 10

### ld
```
	in(immediate|address) value to read.
	out(register) which register to put the read value.
```
* Load value at address PC + in(1) from memory to out(1) register.
* Modify zero flag.
* cycles : 5

### st
```
	in(register) which register contains the value to write
	out(register|address) where to write.
```
* Store in(1) register value to PC+out(1) address in memory.
* cycles : 5

### add
```
	in(register) value
	in(register) value
	out(register) where to write the result
```
* execute an **add** between in(1) and in(2), put the result in out(1)
* Modify zero flag.
* cycles : 10

### sub
```
	in(register) value
	in(register) value
	out(register) where to write the result
```
* execute a **sub** between in(1) and in(2), put the result in out(1)
* Modify zero flag.
* cycles : 10

### and
```
	in(register|immediate|address) value
	in(register|immediate|address) value
	out(register) where to write the result
```

* execute a **and** between in(1) and in(2), put the result in out(1)
* Modify zero flag.
* cycles : 6

### or
```
	in(register|immediate|address) value
	in(register|immediate|address) value
	out(register) where to write the result
```
* execute a **or** between in(1) and in(2), put the result in out(1)
* Modify zero flag.
* cycles : 6

### xor
```
	in(register|immediate|address) value
	in(register|immediate|address) value
	out(register) where to write the result
```
* execute a **xor** between in(1) and in(2), put the result in out(1)
* Modify zero flag.
* cycles : 6

### jnz
```
	in(immediate) offset of the jump
```
* **Does not have an argument coding byte**.
* add in(1) to caller PC **if** zero flag set to 1.
* argument is an address size.
* cycles : 20

### ldi
```
	in(immediate|address|register) address of value to read.
	in(immediate|register) offset to add in(1)
	out(register) which register to put the read value.
```
* Load value at address PC + in(1) + in(2) from memory, write to out(1) register.
* Modify zero flag.
* All values are address size.
* cycles : 25

### sti
```
	in(register) which register contains the value to write
	out(immediate|address|register) address of where to write.
	out(immediate|register) offset to add out(1)
```
* Store in(1) register value to PC+out(1)+out(2) address in memory.
* All values are address size.
* cycles : 25

### fork
```
	in(immediate) offset to add to caller PC
```
* **Does not have an argument coding byte**.
* Create a new process, copying the registers and flags from the caller, the new process PC is *caller* PC + in(1)
* Values are address size.
* cycles : 800

## lld
```
	in(immediate|address) value to read.
	out(register) which register to put the read value.
```
* Load value at address PC + in(1) from memory to out(1) register.
* Address are **not** modulo 512.
* Modify zero flag.
* cycles : 10


## lldi
```
	in(immediate|address|register) address of value to read.
	in(immediate|register) offset to add in(1)
	out(register) which register to put the read value.
```
* Load value at address PC + in(1) + in(2) from memory, write to out(1) register.
* Address are **not** modulo 512.
* Modify zero flag.
* All values are address size.
* cycles : 50

### lfork
```
	in(immediate) offset to add to caller PC
```
* **Does not have an argument coding byte**.
* Create a new process, copying the registers and flags from the caller, the new process PC is *caller* PC + in(1)
* Values are address size.
* Address are **not** modulo 512.
* cycles : 1000

### aff
```
	in(register) value to display
```
* Display the ASCII character in(1) in console.
* cycles : 2

### gtmd
```
	in(register) which register to put the value in
```
* Put the remaining cycles before the memory barrier open into in(1).
* Modify zero flag.
* cycles : 5
