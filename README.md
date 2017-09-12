# BytecodeVM
A basic Virtual machine operating on bytecode.

It operates on a stack and shares memory for instructions and data (van neumann architecture).

It can compile to a binary file and run from a file, or compile and run directly from the assembly file.

Currently a command line program but I plan to integrate it into ETEngine as a visual node based programming language.
Commands:
 * compile [filename.bca] compiles a .bca assembly file to a binary .bce executable
 * run [filename.bce] runs a bytecode executable file
 * cRun [filename.bca] compiles and directly runs an assembly file without saving the executable

### Instruction Set
| Opcode | Description | 
|:----------:|-------------|
| LITERAL | Push next 4 bytes |
| LITERAL_ARRAY | Get x from next 4 bytes; Push x sets of 4 bytes - temporary |
| LOAD ; LOAD_ARG ; LOAD_LCL | Pop a; Push RAM[a] |
| STORE ; STORE_LCL | Pop b; Pop a; RAM[b] = a |
| ADD | Pop b; Pop a; Push a + b |
| SUB | Pop b; Pop a; Push a - b |
| LESS | Pop b; Pop a; Push a < b |
| GREATER | Pop b; Pop a; Push a > b |
| NOT | Pop a; Push !a |
| EQUALS | Pop b; Pop a; Push a == b |
| JMP | Pop a; goto a; |
| JMP_IF | Pop b; Pop a; if a goto b |
| CALL | put current state in a stack frame; store RTN; Pop a; goto a; |
| RETURN | Restore to previous stack frame; append working stack; goto RTN |
| PRINT | Pop x; for x Print Pop - temporary, will be a library function based on null terminated strings |
| PRINT_INT | Pop a; Print string of a |
| PRINT_ENDL | Start a new line in console |

LOAD and STORE have segment modifiers that can be used as base addresses within functions

| Segment | Description |
|:----------:|-------------|
| LCL | pointer to first local variable |
| ARG | pointer to first argument |

| Variable Type | Description |
|:----------:|-------------|
| static | a fixed address variable, referenced directly |
| argument | a variable that sits in the working stack of the previous function's frame |
| local | a local variable within the stack frame of the current function |

There is no concept of scope so all variables should be unique
Variables are declared implicitly upon their first occurance with the exception of arguments
Variables declared before the first function are static, after the first function they are local

The assembler also features Symbols (variables and labels)
 * Variables start with # and are statically allocated at compile time
 * Jump labels start with @
 * Subroutines start with $ and are followed by argument declarations

### Planned

I plan to add:
 * indexed library function support (executes function at pushed index)
 * Dynamic memory allocation
 * Support for standard types int float char bool (maybe short, long, double etc) unsigned or signed
 * Built in support for variable length arrays, strings and vectors
