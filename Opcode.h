#pragma once

enum class Opcode : char
{
    LITERAL,
    PRINT
};

static int TestBytecodeSize = 27;
static char TestBytecode[] = 
{
    (char)Opcode::LITERAL,  
    'H', 
    (char)Opcode::LITERAL,  
    'e', 
    (char)Opcode::LITERAL,  
    'l', 
    (char)Opcode::LITERAL,  
    'l', 
    (char)Opcode::LITERAL,  
    'o', 
    (char)Opcode::LITERAL,  
    ' ', 
    (char)Opcode::LITERAL,  
    'W', 
    (char)Opcode::LITERAL,  
    'o', 
    (char)Opcode::LITERAL,  
    'r', 
    (char)Opcode::LITERAL,  
    'l', 
    (char)Opcode::LITERAL,  
    'd', 
    (char)Opcode::LITERAL,  
    '!', 
    (char)Opcode::LITERAL,  
    static_cast<char>(12), 
    (char)Opcode::PRINT
};
