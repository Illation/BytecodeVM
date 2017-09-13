#pragma once

#include <map>
#include <string>

enum class Opcode : char
{
	//Memory Manipulation
    LITERAL,
    LITERAL_ARRAY,

    LOAD,
    STORE,
    LOAD_LCL,
    STORE_LCL,
    LOAD_ARG,
	
	ALLOC,
	FREE,

	//Arithmetic / Logic
    ADD,
    SUB,

    LESS,
    GREATER,
    NOT,
    EQUALS,

	//Flow Control
    JMP,
    JMP_IF,

	CALL,
	RETURN,

    PRINT,
    PRINT_INT,
    PRINT_ENDL
};
static std::map<std::string, Opcode> OpcodeNames
{
    {"LITERAL", Opcode::LITERAL},
    {"LITERAL_ARRAY", Opcode::LITERAL_ARRAY},

    {"LOAD", Opcode::LOAD},
    {"STORE", Opcode::STORE},
    {"LOAD_LCL", Opcode::LOAD_LCL},
    {"STORE_LCL", Opcode::STORE_LCL},
    {"LOAD_ARG", Opcode::LOAD_ARG},

    {"ALLOC", Opcode::ALLOC},
    {"FREE", Opcode::FREE},

    {"ADD", Opcode::ADD},
    {"SUB", Opcode::SUB},

    {"LESS", Opcode::LESS},
    {"GREATER", Opcode::GREATER},
    {"NOT", Opcode::NOT},
    {"EQUALS", Opcode::EQUALS},

    {"JMP", Opcode::JMP},
    {"JMP_IF", Opcode::JMP_IF},

    {"CALL", Opcode::CALL},
    {"RETURN", Opcode::RETURN},
    
    {"PRINT", Opcode::PRINT},
    {"PRINT_INT", Opcode::PRINT_INT},
    {"PRINT_ENDL", Opcode::PRINT_ENDL}
};
std::string GetOpString(Opcode code);
