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
    void Pack(uint32 address, T value);

private:
    //Static Sizes
    static const uint32 MAX_RAM = 536870912; //500 MB
    uint32 m_StackSize;
    uint32 m_NumInstructions = 0;
    uint32 m_HeapBase = 0;

    bool ProgramLoaded = false;

    //RAM
    uint8* m_RAM;

    //Registers
    uint32 m_ProgramCounter = 0;

    int32 m_StackPointer = -4; //This could also be a static variable
};
