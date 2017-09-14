#pragma once

#include <vector>
#include <string>

#include "AtomicTypes.h"

class VirtualMachine
{
public:
    VirtualMachine();
    ~VirtualMachine();

    bool LoadProgram(std::string filename);
    void SetProgram(std::vector<uint8> bytecode);

    void Interpret(); 

private:
    //Stack Manipulation
    void Push(int32 value);
    int32 Pop();

    //Manipulate memory with 4 bytes
    template<typename T>
    T Unpack(uint32 address);
    template<typename T>
    T Unpack(uint32 address, std::vector<uint8> data);
    template<typename T>
    void Pack(uint32 address, T value);

private:
    //Static Sizes
    static const uint32 MAX_RAM = 536870912; //500 MB
    uint32 m_StackSize;
    uint32 m_NumInstructions = 0;
	uint32 m_StaticBase = 0;
    uint32 m_HeapBase = 0;

	//State
    bool ProgramLoaded = false;

    //RAM
    uint8* m_RAM;

    //Registers
    uint32 m_ProgramCounter = 0;

	//Stack frame, potentially static variables if so desired
	//***********
    int32 m_StackPointer = -4; 
	uint32 m_LCL = 0;	//Current local base address
	uint32 m_ARG = 0;	//Current argument base address
	uint32 m_RTN = 0;	//Current return address
	uint32 m_THIS = 0;	//Pointer to current object

	//Stack Frame Layout for function with n arguments and k locals
	/*
		ARG->	arg 0		****Arguments
				arg 1		<--all arguments are items from previous functions working stack	
				...
				arg n-1
		LCL-16->saved RTN	****Stack frame begin
				saved LCL
				saved ARG
				saved THIS	<--for object support
		LCL->	loc 0		****Local variables
				loc 1
				loc ...
				loc k-1
				ws 0		****Working Stack
		SP->	ws 1
	*/

	//Dynamic Memory Allocation
	//***************
	uint32 m_FirstSegmentPtr = 0;
};
