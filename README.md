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
| LOAD | Pop a; Push RAM[a] |
| STORE | Pop b; Pop a; RAM[b] = a |
| ADD | Pop b; Pop a; Push a + b |
| SUB | Pop b; Pop a; Push a - b |
| LESS | Pop b; Pop a; Push a < b |
| GREATER | Pop b; Pop a; Push a > b |
| NOT | Pop a; Push !a |
| EQUALS | Pop b; Pop a; Push a == b |
| JMP | Pop a; goto a; |
| JMP_IF | Pop b; Pop a; if a goto b |
| PRINT | Pop x; for x Print Pop - temporary, will be a library function based on null terminated strings |
| PRINT_INT | Pop a; Print string of a |
| PRINT_ENDL | Start a new line in console

The assembler also features Symbols (variables and labels)
 * Variables start with # and are statically allocated at compile time
 * Jump labels start with @

### Planned

I plan to add:
 * indexed library function support (executes function at pushed index)
 * Subroutines
 * Dynamic memory allocation
 * Support for standard types int float char bool (maybe short, long, double etc) unsigned or signed
 * Built in support for variable length arrays, strings and vectors
