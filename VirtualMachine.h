#pragma once

class VirtualMachine
{
public:
    VirtualMachine();
    ~VirtualMachine();

    void Interpret(char* bytecode, int size); 

private:
    //Stack Manipulation
    void Push(char value);
    char Pop();

private:
    static const int MAX_STACK = 1024;
    int m_StackPointer=-1;
    char* m_Stack;

};
