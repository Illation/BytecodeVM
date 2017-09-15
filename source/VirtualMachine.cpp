#include "VirtualMachine.h"

#include <fstream>
#include <iterator>
#include <cassert>
#include <iostream>

#include "Opcode.h"
#include "AtomicTypes.h"
#include <limits>

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
	uint32 headerSize = sizeof(uint32)*2;
	m_StackSize = Unpack<uint32>(0, bytecode); 
	uint32 numStaticVars = Unpack<uint32>(1 * sizeof(uint32), bytecode);

    m_NumInstructions = bytecode.size()- headerSize; //Header size for now
	m_StaticBase = m_NumInstructions + m_StackSize;
    for(uint32 i = 0; i < m_NumInstructions; ++i)
    {
        m_RAM[i+m_StackSize] = bytecode[i+ headerSize];
    } 

	//Initialize Dynamic memory allocation
	m_FirstSegmentPtr = m_StaticBase + numStaticVars;
	m_HeapBase = m_FirstSegmentPtr+sizeof(uint32);
	Pack<uint32>(m_FirstSegmentPtr, m_HeapBase);
	Pack<uint32>(m_HeapBase, MAX_RAM - m_HeapBase);
	Pack<uint32>(m_HeapBase+sizeof(uint32), 0);

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
    while( m_ProgramCounter < m_StaticBase)
    {
        assert(m_ProgramCounter - m_StackSize < m_NumInstructions);

        Opcode operation = static_cast<Opcode>(m_RAM[m_ProgramCounter]);

		//std::cout << "[DBG] operation: " << GetOpString(operation) << std::endl;

        switch(operation)
        {
            //MEMORY OPERATIONS
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

            //Mark (a) bytes on the heap as used and push a pointer to the base
            case Opcode::ALLOC:
            {
				uint32 requestedSize = Pop(); 
				uint32 requiredSize = requestedSize + sizeof(uint32);//First 4 bytes of segment hold segment size -- maybe in future 4 more bytes for reference count

				uint32 firstSegment = Unpack<uint32>(m_FirstSegmentPtr);
				uint32 nextSegment = firstSegment;

				uint32 bestFitSize = std::numeric_limits<uint32>::max();
				uint32 bestFitPtr = 0;
				uint32 prevNextPtr = m_FirstSegmentPtr;

				//Get best fitting segment
				bool earlyOut = false;
				while (nextSegment != 0 && !earlyOut)
				{
					uint32 segmentSize = Unpack<uint32>(nextSegment);
					if (segmentSize >= requiredSize && segmentSize < bestFitSize)
					{
						if (segmentSize == requiredSize) earlyOut = true;
						bestFitPtr = nextSegment;
						bestFitSize = segmentSize;
						if(nextSegment != firstSegment)prevNextPtr = nextSegment + sizeof(uint32);
					}
					nextSegment = Unpack<uint32>(nextSegment + sizeof(uint32));
				}
				if (bestFitPtr == 0)
				{
					std::cerr << "[VM] Out of Memory Exception, could not allocate space for variable!" << std::endl;
					return;
				}
				//use best found segment
				uint32 remainingSize = bestFitSize - requiredSize;
				if (remainingSize >= sizeof(uint32)*2)//Split segment in two if the remainder is big enough to allocate (ie its bigger than a segment header)
				{
					uint32 newSegPtr = bestFitPtr + requiredSize;
					Pack<uint32>(newSegPtr, remainingSize); //set the new segment size
					Pack<uint32>(newSegPtr + sizeof(uint32), Unpack<uint32>(bestFitPtr+sizeof(uint32))); //set the new segments nextPtr to the value of the allocated segments next ptr
					Pack<uint32>(prevNextPtr, newSegPtr);//Link the previous segment to the new segment

					Pack<uint32>(bestFitPtr, requiredSize); //Tell the allocated segment how big it is
				}
				else //Allocate the entire segment
				{
					Pack<uint32>(prevNextPtr, Unpack<uint32>(bestFitPtr+sizeof(uint32)));//Link the previous segment to next segment
				}
				Push(bestFitPtr+sizeof(uint32));
            }
            continue;
            //Mark the space at (a) as unused
            case Opcode::FREE:
            {
				uint32 segmentPtr = Pop()-sizeof(uint32);
				uint32 segmentSize = Unpack<uint32>(segmentPtr);

				uint32 firstSegment = Unpack<uint32>(m_FirstSegmentPtr);
				uint32 existingSegment = firstSegment;

				uint32 prevNextPtr = m_FirstSegmentPtr;
				bool earlyOut = false;
				while (existingSegment != 0 && !earlyOut)
				{
					if (segmentPtr > existingSegment)
					{
						//insert
						bool standalone = true;
						uint32 existingSize = Unpack<uint32>(existingSegment);
						if (existingSegment + existingSize == segmentPtr)//Merge EXISTING+INSERTED
						{
							//simply expand the existing segment to accomodate our size
							segmentPtr = existingSegment;
							segmentSize += existingSize;
							Pack<uint32>(segmentPtr, segmentSize);
							standalone = false;
						}
						else
						{

						}
						existingSegment = Unpack<uint32>(existingSegment + sizeof(uint32));
						if (segmentPtr + segmentSize == existingSegment)//Merge INSERTED+NEXT
						{
							segmentSize += Unpack<uint32>(existingSegment);
							Pack<uint32>(segmentPtr, segmentSize);
							Pack<uint32>(segmentPtr + sizeof(uint32), Unpack<uint32>(existingSegment + sizeof(uint32)));
						}
						else if(standalone) Pack<uint32>(segmentPtr + sizeof(uint32), existingSegment);
						earlyOut = true;
					}
				}
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
			
			//FUNCTIONS
			//put a new frame on the stack with n arguments and k local variables
			case Opcode::CALL:
            {
				uint32 ret = m_ProgramCounter + 1;
				m_ProgramCounter = static_cast<uint32>(Pop());
				Push(m_RTN);
				m_RTN = ret;
				Push(m_LCL);
				Push(m_ARG);
				Push(m_THIS); //This stays the same because we are doing a function not a method
				m_ARG = m_StackPointer - (Unpack<int32>(m_ProgramCounter) + 12/*difference from this to return*/);
				m_ProgramCounter += sizeof(int32);
				m_LCL = m_StackPointer + sizeof(int32);
				m_StackPointer = m_LCL + Unpack<int32>(m_ProgramCounter);
				m_ProgramCounter += sizeof(int32);
            }
            continue;
			//Return from current function to previous function on stack and copy end values over
			case Opcode::RETURN: //#todo stop assuming return value size
            {
                m_ProgramCounter = m_RTN;
				Pack<int32>(m_ARG, Pop());
				m_StackPointer = m_ARG;
				m_THIS = Unpack<int32>(m_LCL - (sizeof(int32) * 1));
				m_ARG = Unpack<int32>(m_LCL - (sizeof(int32) * 2));
				m_RTN = Unpack<int32>(m_LCL - (sizeof(int32) * 4));
				m_LCL = Unpack<int32>(m_LCL - (sizeof(int32) * 3));
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
    assert(m_StackPointer + sizeof(int32) < m_StackSize); //Stack Overflow
    Pack<int32>(m_StackPointer+=sizeof(int32), value);
}
int VirtualMachine::Pop()
{
    assert(m_StackPointer >= 0); //Invalid memory access "Stack underflow" - this does not protect against the SP underflowing the working stack
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
T VirtualMachine::Unpack(uint32 address, std::vector<uint8> data)
{
	assert(data.size() > address + 3);
#ifdef WORD_BIG_ENDIAN
    uint32 value =	static_cast<uint8>(data[address+3]) << 24 |
					static_cast<uint8>(data[address+2]) << 16 |
					static_cast<uint8>(data[address+1]) << 8 |
					static_cast<uint8>(data[address+0]);
#elif
    uint32 value =  static_cast<uint8>(data[address+0]) << 24 |
					static_cast<uint8>(data[address+1]) << 16 |
					static_cast<uint8>(data[address+2]) << 8 |
					static_cast<uint8>(data[address+3]);
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
