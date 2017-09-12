#include "VirtualMachine.h"

#include <fstream>
#include <iterator>
#include <cassert>
#include <iostream>

#include "Opcode.h"
#include "AtomicTypes.h"

VirtualMachine::VirtualMachine()
{
    m_RAM = new uint8[MAX_RAM];
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
    std::vector<uint8> bytecode;
    bytecode.reserve(static_cast<uint32>(fileSize));
    
    // read the data:
    bytecode.insert(bytecode.begin(),
        std::istream_iterator<uint8>(file),
        std::istream_iterator<uint8>());

    SetProgram(bytecode);

    return true;
}
void VirtualMachine::SetProgram(std::vector<uint8> bytecode)
{
#ifdef WORD_BIG_ENDIAN
    m_StackSize =   static_cast<uint8>(bytecode[3]) << 24 |
                    static_cast<uint8>(bytecode[2]) << 16 |
                    static_cast<uint8>(bytecode[1]) << 8 |
                    static_cast<uint8>(bytecode[0]);
#elif
    m_StackSize =   static_cast<uint8>(bytecode[0]) << 24 |
                    static_cast<uint8>(bytecode[1]) << 16 |
                    static_cast<uint8>(bytecode[2]) << 8 |
                    static_cast<uint8>(bytecode[3]);
#endif

    m_NumInstructions = bytecode.size()-sizeof(int32); //Header size for now
    m_HeapBase = m_NumInstructions + m_StackSize;
    for(uint32 i = 0; i < m_NumInstructions; ++i)
    {
        m_RAM[i+m_StackSize] = bytecode[i+sizeof(int32)];
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
        assert(m_ProgramCounter - m_StackSize < m_NumInstructions);

        Opcode operation = static_cast<Opcode>(m_RAM[m_ProgramCounter]);

        switch(operation)
        {
            //Add a byte to the stack
            case Opcode::LITERAL:
            {
                Push(Unpack<int32>(++m_ProgramCounter));
                m_ProgramCounter+=sizeof(int32);
            }
            continue;

            //Add multiple bytes to the stack
            case Opcode::LITERAL_ARRAY:
            {
                int numValues = Unpack<int32>(++m_ProgramCounter);
                m_ProgramCounter+=sizeof(int32);
                while(numValues > 0)
                {
                    Push(Unpack<int32>(m_ProgramCounter));
                    m_ProgramCounter+=sizeof(int32);
                    --numValues;
                }
            }
            continue;

            //MEMORY OPERATIONS
            //put memory at address on stack
            case Opcode::LOAD:
            {
                Push(Unpack<int32>(Pop()));
                ++m_ProgramCounter;
            }
            continue;
            //store a in memory at b
            case Opcode::STORE:
            {
                int32 address = Pop();
                Pack<int32>(address, Pop());
                ++m_ProgramCounter;
            }
            continue;
            //put memory at local address on stack
            case Opcode::LOAD_LCL:
            {
                Push(Unpack<int32>(m_LCL+Pop()));
                ++m_ProgramCounter;
            }
            continue;
            //store a in memory at local b
            case Opcode::STORE_LCL:
            {
                int32 address = m_LCL+Pop();
                Pack<int32>(address, Pop());
                ++m_ProgramCounter;
            }
            continue;
            //put memory at argument address on stack
            case Opcode::LOAD_ARG:
            {
                Push(Unpack<int32>(m_ARG+Pop()));
                ++m_ProgramCounter;
            }
            continue;

            //ARITHMETIC OPERATIONS
            //Add values together
            case Opcode::ADD:
            {
                int32 b = Pop();
                int32 a = Pop();
                Push(a + b);
                ++m_ProgramCounter;
            }
            continue;
            //a - b
            case Opcode::SUB:
            {
                int32 b = Pop();
                int32 a = Pop();
                Push(a - b);
                ++m_ProgramCounter;
            }
            continue;

            //LOGICAL OPERATIONS
            //a < b
            case Opcode::LESS:
            {
                int32 b = Pop();
                int32 a = Pop();
                Push(a < b);
                ++m_ProgramCounter;
            }
            continue;
            //a > b
            case Opcode::GREATER:
            {
                int32 b = Pop();
                int32 a = Pop();
                Push(a > b);
                ++m_ProgramCounter;
            }
            continue;
            //!a
            case Opcode::NOT:
            {
                int32 a = Pop();
                Push(!a);
                ++m_ProgramCounter;
            }
            continue;
            //a == b
            case Opcode::EQUALS:
            {
                int32 b = Pop();
                int32 a = Pop();
                Push(a == b);
                ++m_ProgramCounter;
            }
            continue;

            //FLOW CONTROL
            //goto a
            case Opcode::JMP:
            {
                int32 address = Pop();
                m_ProgramCounter = static_cast<uint32>(address);
            }
            continue;
            //if(a) goto b
            case Opcode::JMP_IF:
            {
                int32 address = Pop();
                int32 condition = Pop();
                if(condition)
                {
                    m_ProgramCounter = static_cast<uint32>(address);
                }
                else
                {
                    ++m_ProgramCounter;
                }
            }
            continue;

            //"Library functions" should later be implemented differently
            //print x chars to console
            case Opcode::PRINT:
            {
                uint32 size = Pop();
                std::string out;
                for(uint32 j = 0; j<size; ++j)
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

            //INVALID
            default:
            std::cerr << "Invalid opcode: " << GetOpString(operation) << std::endl;
            assert(false);
            continue;
        }
    }
}

void VirtualMachine::Push(int32 value)
{
    assert(m_StackPointer + sizeof(int32) < (int32)m_StackSize); //Stack Overflow
    Pack<int32>(m_StackPointer+=sizeof(int32), value);
}
int VirtualMachine::Pop()
{
    assert(m_StackPointer >= 0); //Invalid memory access "Stack underflow"
    int32 value = Unpack<int32>(m_StackPointer);
    m_StackPointer -= sizeof(int32);
    return value;
}

template<typename T>
T VirtualMachine::Unpack(uint32 address)
{
#ifdef WORD_BIG_ENDIAN
    uint32 value =	static_cast<uint8>(m_RAM[address+3]) << 24 |
					static_cast<uint8>(m_RAM[address+2]) << 16 |
					static_cast<uint8>(m_RAM[address+1]) << 8 |
					static_cast<uint8>(m_RAM[address+0]);
#elif
    uint32 value =  static_cast<uint8>(m_RAM[address+0]) << 24 |
					static_cast<uint8>(m_RAM[address+1]) << 16 |
					static_cast<uint8>(m_RAM[address+2]) << 8 |
					static_cast<uint8>(m_RAM[address+3]);
#endif
    return static_cast<T>(value);
}
template<typename T>
void VirtualMachine::Pack(uint32 address, T value)
{
   uint32 n = static_cast<uint32>(value);
#ifdef WORD_BIG_ENDIAN
   m_RAM[address+3] = (n >> 24) & 0xFF;
   m_RAM[address+2] = (n >> 16) & 0xFF;
   m_RAM[address+1] = (n >> 8) & 0xFF;
   m_RAM[address+0] = n & 0xFF;
#elif //WORD_LITTLE_ENDIAN
   m_RAM[address+0] = (n >> 24) & 0xFF;
   m_RAM[address+1] = (n >> 16) & 0xFF;
   m_RAM[address+2] = (n >> 8) & 0xFF;
   m_RAM[address+3] = n & 0xFF;
#endif
}
