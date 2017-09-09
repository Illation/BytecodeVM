#pragma once

#include <map>
#include <string>

enum class Opcode : char
{
    LITERAL,
    LITERAL_ARRAY,

    LOAD,
    STORE,

    ADD,
    SUB,

    LESS,
    GREATER,
    NOT,
    EQUALS,

    JMP,
    JMP_IF,

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

    {"ADD", Opcode::ADD},
    {"SUB", Opcode::SUB},

    {"LESS", Opcode::LESS},
    {"GREATER", Opcode::GREATER},
    {"NOT", Opcode::NOT},
    {"EQUALS", Opcode::EQUALS},

    {"JMP", Opcode::JMP},
    {"JMP_IF", Opcode::JMP_IF},
    
    {"PRINT", Opcode::PRINT},
    {"PRINT_INT", Opcode::PRINT_INT},
    {"PRINT_ENDL", Opcode::PRINT_ENDL}
};
std::string GetOpString(Opcode code);
