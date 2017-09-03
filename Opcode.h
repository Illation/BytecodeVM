#pragma once

#include <map>
#include <string>

enum class Opcode : char
{
    LITERAL,
    LITERAL_ARRAY,
    PRINT
};
static std::map<std::string, Opcode> OpcodeNames
{
    {"LITERAL", Opcode::LITERAL},
    {"LITERAL_ARRAY", Opcode::LITERAL_ARRAY},
    {"PRINT", Opcode::PRINT}
};
