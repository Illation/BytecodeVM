#pragma once

#include <map>
#include <string>

enum class Opcode : char
{
    LITERAL,
    PRINT
};
static std::map<std::string, Opcode> OpcodeNames
{
    {"LITERAL", Opcode::LITERAL},
    {"PRINT", Opcode::PRINT}
};
