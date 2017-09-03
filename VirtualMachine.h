#pragma once

#include <vector>
#include <string>

class VirtualMachine
{
public:
    VirtualMachine();
    ~VirtualMachine();

    bool LoadProgram(std::string filename);
    void SetProgram(std::vector<char> bytecode);

    void Interpret(); 

private:
    //Stack Manipulation
    void Push(char value);
    char Pop();

private:
    static const int MAX_STACK = 1024;
    int m_StackPointer=-1;
    char* m_Stack;

    std::vector<char> m_Bytecode;
    bool ProgramLoaded = false;
};
