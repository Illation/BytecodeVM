#pragma once

#include <string>
#include <vector>

#include "AtomicTypes.h"

//Forward declaration
class SymbolTable;

class AssemblyCompiler
{
public:
    enum class CompState
    {
        INIT,
        SOURCE,
        COMPILED,
        FAILED
    };

public:
    AssemblyCompiler();
    ~AssemblyCompiler();

    void SetSource(std::vector<std::string> lines);
    bool LoadSource(std::string filename);

    bool Compile();

    CompState GetState(){return m_State;}
    bool Save(std::string filename);
    std::vector<uint8> GetBytecode();

private:
    bool BuildSymbolTable();
    bool CompileInstructions();
    bool CompileHeader();

    bool TokenizeLine(std::string line, std::string &opname, std::string &arguments);
    bool IsValidOpname(std::string opname, uint32 line);

    void CheckVar(std::string &arguments);

    bool HasValidArgs(std::string arguments, uint32 line, std::string opname);
    bool ParseLiteral(int32 &out, std::string &arguments);
    void WriteInt(int32 value);
    void WriteInt(int32 value, std::vector<uint8> &target);

    void PrintAbort(uint32 line);

private:
    CompState m_State = CompState::INIT;

    std::vector<std::string> m_Lines;
    std::vector<uint8> m_Bytecode;

	SymbolTable* m_pSymbolTable = nullptr;

    uint32 m_HeaderSize = 0;
    uint32 m_StackSize = 1048576;
};
