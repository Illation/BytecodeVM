#include "VirtualMachine.h"

#include <cassert>
#include <string>
#include <iostream>

#include "Opcode.h"

VirtualMachine::VirtualMachine()
{
    m_Stack = new char[MAX_STACK];
}
VirtualMachine::~VirtualMachine()
{
    delete[] m_Stack;
}

void VirtualMachine::Interpret(char* bytecode, int size)
{
    for(unsigned int i = 0; i < size; ++i)
    {
        Opcode operation = static_cast<Opcode>(bytecode[i]);
        switch(operation)
        {
            //Add a byte to the stack
            case Opcode::LITERAL:
            {
                char value = bytecode[++i];
                Push(value);
            }
            continue;

            //print x chars to console
            case Opcode::PRINT:
            {
                unsigned int size = Pop();
                std::string out;
                for(int j = 0; j<size; ++j)
                {
                    out = Pop() + out;
                }
                std::cout << out << std::endl;
            }
            continue;
        }
    }
}

void VirtualMachine::Push(char value)
{
    assert(m_StackPointer + 1 < MAX_STACK); 
    m_Stack[++m_StackPointer] = value;
}
char VirtualMachine::Pop()
{
    assert(m_StackPointer >= 0); 
    return m_Stack[m_StackPointer--];
}
