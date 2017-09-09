#pragma once

#include <vector>
#include <string>

class VirtualMachine
{
public:
    VirtualMachine(unsigned int stackSize = 1048576);
    ~VirtualMachine();

    bool LoadProgram(std::string filename);
    void SetProgram(std::vector<char> bytecode);

    void Interpret(); 

private:
    //Stack Manipulation
    void Push(int value);
    int Pop();

    //Manipulate memory with 4 bytes
    template<typename T>
    T Unpack(unsigned int address);
    template<typename T>
    void Pack(unsigned int address, T value);

private:
    //Static Sizes
    static const int MAX_RAM = 536870912; //500 MB
    unsigned int m_StackSize = 1024;
    unsigned int m_NumInstructions = 0;
    unsigned int m_HeapBase = 0;

    //RAM
    char* m_RAM;

    //Registers
    unsigned int m_ProgramCounter = 0;
    int m_StackPointer = -4;

    bool ProgramLoaded = false;
};
