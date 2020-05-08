# CSE 509 Homework 3 by Benjamin Michalowicz and Ian Buitenkant

### Setup
0. Have pin downloaded. ```pin-3.13-98189-g60a6ef199-gcc-linux```is the version used here, just in case.
1. Pin directory is in the ~/ directory (/home/sekar)
2. Place tar-file into  in ~/(pinDIR)/source/tools/
3. When untar-ing, you will get a folder 'CSE509HW3'
4. Running "make" or "make all" should compile code for all projects 


## Part 1: Pintools Warmups


### Pintool 1: Malloc Tracer/Counter
 Counts the number of ```Malloc(3)``` calls used in an application, their return values, and at the end of the file, writes the number of bytes alocated.

#### Usage:
From inside ```pin-3.13-98189-g60a6ef199-gcc-linux/source/tools/CSE509HW3```: ```../../../pin -t obj-ia32/pinMalloc.so -- <Your Application>```

#### Errata:
Firefox doesn't seem to want to return the number of allocated calls or bytes allocated, even after waiting for a full loading of the application.

### Pintool 2: Basic Block Counter
Counts the number of Basic Blocks (code blocks without any jumps or branches to other code pieces) executed in a program.
.
#### Usage
From inside ```pin-3.13-98189-g60a6ef199-gcc-linux/source/tools/CSE509HW3```: ```../../../pin -t obj-ia32/pinBlock.so -- <Your Application> ```

#### Errata:
Firefox still doesn't want to return anything, unlike other arbitrary programs like ```ls```, ```vim```, etc.


### Pintool 3: Direct/Indirect Control Transfer/Return/Branch Count
Counts all instances of direct/indirect control flow. This includes branches, returns, function calls, and syscalls.

#### Usage
From inside ```pin-3.13-98189-g60a6ef199-gcc-linux/source/tools/CSE509HW3```: ```../../../pin -t obj-ia32/pinCtrl.so -- <Your Application> ```

#### ErrataL

## Part 2: Security Application: "bTraceApp", or p(in)Trace
	Performs system call interception with Pin calls inside Basic Blocks
#### Usage:
From inside ```pin-3.13-98189-g60a6ef199-gcc-linux/source/tools/CSE509HW3```: ```../../../pin -t obj-ia32/btraceApp.so -- <Your Application> ```

#### Breakdown
Breaks down basic blocks in a trace, where if a given instruction is a system call, every argument w.r.t said call is printed, along with an associated name. 
The start of the next basic block is after the return from kernel space, which allows the return value from EAX to be processed. Error numbers are provided if the returned ```errno``` variable is NOT zero.

#### Errata


