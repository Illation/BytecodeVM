#include "VirtualMachine.h"

#include <fstream>
#include <iterator>
#include <cassert>
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

bool VirtualMachine::LoadProgram(std::string filename)
{
    std::ifstream file( filename, std::ios::binary );
    if(!file.good())
    {
        std::cerr << "[VM] Could not open bytecode executable" << std::endl;
        return false;
    }

    file.unsetf(std::ios::skipws);

    // get its size:
    std::streampos fileSize;
    
    file.seekg(0, std::ios::end);
    fileSize = file.tellg();
    file.seekg(0, std::ios::beg);
    
    // reserve capacity
    m_Bytecode.reserve(fileSize);
    
    // read the data:
    m_Bytecode.insert(m_Bytecode.begin(),
        std::istream_iterator<char>(file),
        std::istream_iterator<char>());

    ProgramLoaded = true;
    return true;
}
void VirtualMachine::SetProgram(std::vector<char> bytecode)
{
    m_Bytecode = bytecode;
    ProgramLoaded = true;
}

void VirtualMachine::Interpret()
{
    if(!ProgramLoaded)
    {
        std::cerr << "[VM] No program loaded" << std::endl;
        return;
    }

    for(unsigned int i = 0; i < m_Bytecode.size(); ++i)
    {
        Opcode operation = static_cast<Opcode>(m_Bytecode[i]);
        switch(operation)
        {
            //Add a byte to the stack
            case Opcode::LITERAL:
            {
                char value = m_Bytecode[++i];
                Push(value);
            }
            continue;

            //Add multiple bytes to the stack
            case Opcode::LITERAL_ARRAY:
            {
                char numValues = m_Bytecode[++i];
                while(numValues > 0)
                {
                    Push(m_Bytecode[++i]);
                    --numValues;
                }
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
