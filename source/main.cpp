#include <iostream>
#include <string>

#include "VirtualMachine.h"
#include "AssemblyCompiler.h"
#include "Opcode.h"

#undef BIG_ENDIAN
#ifndef LITTLE_ENDIAN
    #define LITTLE_ENDIAN
#endif

static const std::string AssemblyExtension(".bca");
static const std::string ExecutableExtension(".bce");

bool hasEnding (std::string const &fullString, std::string const &ending) 
{
    if (fullString.length() >= ending.length()) 
    {
        return (0 == fullString.compare (fullString.length() - ending.length(), ending.length(), ending));
    } 
    else 
    {
        return false;
    }
}

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
        
        //Create a new VM / interpreter
        VirtualMachine* pVM = new VirtualMachine();
        pVM->LoadProgram(filename);
        pVM->Interpret();
        delete pVM;
        pVM = nullptr;
        
        std::cout << std::endl; 
        std::cout << "=======================" << std::endl; 
        std::cout << "script execution ended!" << std::endl; 
    }
    else if(std::string(argv[1]) == "compile")
    {
        std::cout << "compiling " << filename << std::endl; 
        std::cout << std::endl; 

        AssemblyCompiler* pCmp = new AssemblyCompiler();
        pCmp->LoadSource(filename);

        pCmp->Compile();

        std::string outname;
        if(hasEnding(filename, AssemblyExtension))
        {
            outname = filename.substr(0, filename.size() - AssemblyExtension.size()) + ExecutableExtension; 
        } 
        else outname = filename + ExecutableExtension;
        pCmp->Save(outname);

        std::cout << std::endl; 
        std::cout << "=======================" << std::endl; 
        if(!(pCmp->GetState() == AssemblyCompiler::CompState::COMPILED))
        {
            std::cout << "script compilation failed!" << std::endl; 
            delete pCmp; 
            pCmp = nullptr;
            return 3;
        }
        else
        {
            std::cout << "script compilation succeded! >> output file: " << outname << std::endl; 
        }

        delete pCmp; 
        pCmp = nullptr;
    }
    else if(std::string(argv[1]) == "cRun")
    {
        std::cout << "compiling " << filename << std::endl; 
        std::cout << std::endl; 

        AssemblyCompiler* pCmp = new AssemblyCompiler();
        pCmp->LoadSource(filename);

        pCmp->Compile();

        std::cout << std::endl; 
        std::cout << "=======================" << std::endl; 
        if(!(pCmp->GetState() == AssemblyCompiler::CompState::COMPILED))
        {
            std::cout << "script compilation failed!" << std::endl; 
            delete pCmp; 
            pCmp = nullptr;
            return 3;
        }
        std::cout << "running " << filename << std::endl; 
        std::cout << std::endl; 

        VirtualMachine* pVM = new VirtualMachine();

        pVM->SetProgram(pCmp->GetBytecode());

        delete pCmp; 
        pCmp = nullptr;

        pVM->Interpret();

        delete pVM;
        pVM = nullptr;

        std::cout << std::endl; 
        std::cout << "=======================" << std::endl; 

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
