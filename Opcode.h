#pragma once

#include <map>
#include <string>

enum class Opcode : char
{
    LITERAL,
    LITERAL_ARRAY,
    PUSH,
    POP,
    ADD,
    SUB,
    PRINT,
    PRINT_INT
};
static std::map<std::string, Opcode> OpcodeNames
{
    {"LITERAL", Opcode::LITERAL},
    {"LITERAL_ARRAY", Opcode::LITERAL_ARRAY},
    {"PUSH", Opcode::PUSH},
    {"POP", Opcode::POP},
    {"ADD", Opcode::ADD},
    {"SUB", Opcode::SUB},
    {"PRINT", Opcode::PRINT},
    {"PRINT_INT", Opcode::PRINT_INT}
};
