#include <iostream>
#include <string>

#include "VirtualMachine.h"
#include "Opcode.h"

int main(int argc, char** argv)
{
    if(argc != 3)	
    {
        std::cout << "usage: [operation] [filename]" << std::endl; 
        return 1;
    }
    std::string filename = argv[2];
    if(std::string(argv[1]) == "run")
    {
        std::cout << "running " << filename << std::endl; 
        std::cout << std::endl; 
        
        //Load the file as binary info
        char* bytecode = TestBytecode;
        int bytecodeSize = TestBytecodeSize;

        //Create a new VM / interpreter
        VirtualMachine* pVM = new VirtualMachine();
        pVM->Interpret(bytecode, bytecodeSize);
        delete pVM;
        pVM = nullptr;
        
        std::cout << std::endl; 
        std::cout << "=======================" << std::endl; 
        std::cout << "script execution ended!" << std::endl; 
    }
    else
    {
        std::cout << "OPERATION NOT RECOGNIZED!" << std::endl; 
        std::cout << "usage: [operation] [filename]" << std::endl; 
        std::cout << "operations: " << std::endl; 
        std::cout << "\trun >> Run virtual machine with executable bytecode" << std::endl; 
        std::cout << "\tcompile >> compile assembly code to executable bytecode" << std::endl; 
        return 2;
    }
    return 0;
}
