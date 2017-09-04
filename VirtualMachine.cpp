#include "VirtualMachine.h"

#include <fstream>
#include <iterator>
#include <cassert>
#include <iostream>

#include "Opcode.h"

VirtualMachine::VirtualMachine(unsigned int stackSize)
    :m_StackSize(stackSize)
{
    m_RAM = new char[MAX_RAM];
}
VirtualMachine::~VirtualMachine()
{
    delete[] m_RAM;
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
    std::vector<char> bytecode;
    bytecode.reserve(fileSize);
    
    // read the data:
    bytecode.insert(bytecode.begin(),
        std::istream_iterator<char>(file),
        std::istream_iterator<char>());

    SetProgram(bytecode);

    return true;
}
void VirtualMachine::SetProgram(std::vector<char> bytecode)
{
    m_NumInstructions = bytecode.size();
    m_HeapBase = m_NumInstructions + m_StackSize;
    for(int i = 0; i < m_NumInstructions; ++i)
    {
        m_RAM[i+m_StackSize] = bytecode[i];
    } 
    ProgramLoaded = true;
}

void VirtualMachine::Interpret()
{
    if(!ProgramLoaded)
    {
        std::cerr << "[VM] No program loaded" << std::endl;
        return;
    }

    for(m_ProgramCounter = m_StackSize; m_ProgramCounter < m_HeapBase; ++m_ProgramCounter)
    {
        Opcode operation = static_cast<Opcode>(m_RAM[m_ProgramCounter]);
        switch(operation)
        {
            //Add a byte to the stack
            case Opcode::LITERAL:
            {
                char value = m_RAM[++m_ProgramCounter];
                Push(value);
            }
            continue;

            //Add multiple bytes to the stack
            case Opcode::LITERAL_ARRAY:
            {
                char numValues = m_RAM[++m_ProgramCounter];
                while(numValues > 0)
                {
                    Push(m_RAM[++m_ProgramCounter]);
                    --numValues;
                }
            }
            continue;

            //put memory at address on stack
            case Opcode::PUSH:
            {
                Push(m_RAM[Pop()]);
            }
            continue;

            //store a in memory at b
            case Opcode::POP:
            {
                char address = Pop();
                m_RAM[address] = Pop();
            }
            continue;

            //Add values together
            case Opcode::ADD:
            {
                char b = Pop();
                char a = Pop();
                Push(a + b);
            }
            continue;

            //a - b
            case Opcode::SUB:
            {
                char b = Pop();
                char a = Pop();
                Push(a - b);
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
    assert(m_StackPointer + 1 < m_StackSize); 
    m_RAM[++m_StackPointer] = value;
}
char VirtualMachine::Pop()
{
    assert(m_StackPointer >= 0); 
    return m_RAM[m_StackPointer--];
}
