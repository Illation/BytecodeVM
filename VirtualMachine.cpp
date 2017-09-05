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
#ifdef BIG_ENDIAN
    m_StackSize =   static_cast<unsigned char>(bytecode[3]) << 24 |
                    static_cast<unsigned char>(bytecode[2]) << 16 |
                    static_cast<unsigned char>(bytecode[1]) << 8 |
                    static_cast<unsigned char>(bytecode[0]);
#elif
    m_StackSize =   static_cast<unsigned char>(bytecode[0]) << 24 |
                    static_cast<unsigned char>(bytecode[1]) << 16 |
                    static_cast<unsigned char>(bytecode[2]) << 8 |
                    static_cast<unsigned char>(bytecode[3]);
#endif

    m_NumInstructions = bytecode.size()-sizeof(int);
    m_HeapBase = m_NumInstructions + m_StackSize;
    for(int i = 0; i < m_NumInstructions; ++i)
    {
        m_RAM[i+m_StackSize] = bytecode[i+sizeof(int)];
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

    m_ProgramCounter = m_StackSize;
    while( m_ProgramCounter < m_HeapBase)
    {
        Opcode operation = static_cast<Opcode>(m_RAM[m_ProgramCounter]);
        switch(operation)
        {
            //Add a byte to the stack
            case Opcode::LITERAL:
            {
                Push(Unpack<int>(++m_ProgramCounter));
                m_ProgramCounter+=sizeof(int);//need to increment pc additionally as an int occupies multiple instructions
            }
            continue;

            //Add multiple bytes to the stack
            case Opcode::LITERAL_ARRAY:
            {
                int numValues = Unpack<int>(++m_ProgramCounter);
                m_ProgramCounter+=sizeof(int);
                while(numValues > 0)
                {
                    Push(Unpack<int>(m_ProgramCounter));
                    m_ProgramCounter+=sizeof(int);
                    --numValues;
                }
            }
            continue;

            //put memory at address on stack
            case Opcode::PUSH:
            {
                Push(Unpack<int>(Pop()));
                ++m_ProgramCounter;
            }
            continue;

            //store a in memory at b
            case Opcode::POP:
            {
                int address = Pop();
                Pack<int>(address, Pop());
                ++m_ProgramCounter;
            }
            continue;

            //Add values together
            case Opcode::ADD:
            {
                int b = Pop();
                int a = Pop();
                Push(a + b);
                ++m_ProgramCounter;
            }
            continue;

            //a - b
            case Opcode::SUB:
            {
                int b = Pop();
                int a = Pop();
                Push(a - b);
                ++m_ProgramCounter;
            }
            continue;

            //print x chars to console
            case Opcode::PRINT:
            {
                unsigned int size = Pop();
                std::string out;
                for(int j = 0; j<size; ++j)
                {
                    out = static_cast<char>(Pop()) + out;
                }
                std::cout << out;
                ++m_ProgramCounter;
            }
            continue;
            
            //print one integer to console
            case Opcode::PRINT_INT:
            {
                std::cout << Pop();
                ++m_ProgramCounter;
            }
            continue;
            
            //print one integer to console
            case Opcode::PRINT_ENDL:
            {
                std::cout << std::endl;
                ++m_ProgramCounter;
            }
            continue;

            default:
            std::cerr << "Invalid opcode: " << int(static_cast<char>(operation)) << std::endl;
            assert(false);
            continue;
        }
    }
}

void VirtualMachine::Push(int value)
{
    assert(m_StackPointer + sizeof(int) < (int)m_StackSize); 
    Pack<int>(m_StackPointer+=sizeof(int), value);
}
int VirtualMachine::Pop()
{
    assert(m_StackPointer >= 0); 
    int value = Unpack<int>(m_StackPointer);
    m_StackPointer -= sizeof(int);
    return value;
}

template<typename T>
T VirtualMachine::Unpack(unsigned int address)
{
#ifdef BIG_ENDIAN
    unsigned int value =    static_cast<unsigned char>(m_RAM[address+3]) << 24 |
                            static_cast<unsigned char>(m_RAM[address+2]) << 16 |
                            static_cast<unsigned char>(m_RAM[address+1]) << 8 |
                            static_cast<unsigned char>(m_RAM[address+0]);
#elif
    unsigned int value =    static_cast<unsigned char>(m_RAM[address+0]) << 24 |
                            static_cast<unsigned char>(m_RAM[address+1]) << 16 |
                            static_cast<unsigned char>(m_RAM[address+2]) << 8 |
                            static_cast<unsigned char>(m_RAM[address+3]);
#endif
    return static_cast<T>(value);
}
template<typename T>
void VirtualMachine::Pack(unsigned int address, T value)
{
   unsigned int n = static_cast<unsigned int>(value);
#ifdef BIG_ENDIAN
   m_RAM[address+3] = (n >> 24) & 0xFF;
   m_RAM[address+2] = (n >> 16) & 0xFF;
   m_RAM[address+1] = (n >> 8) & 0xFF;
   m_RAM[address+0] = n & 0xFF;
#elif
   m_RAM[address+0] = (n >> 24) & 0xFF;
   m_RAM[address+1] = (n >> 16) & 0xFF;
   m_RAM[address+2] = (n >> 8) & 0xFF;
   m_RAM[address+3] = n & 0xFF;
#endif
}
