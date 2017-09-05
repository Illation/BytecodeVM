#pragma once

#include <string>
#include <vector>

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
    std::vector<char> GetBytecode();

private:
    bool BuildSymbolTable();
    bool CompileInstructions();
    bool CompileHeader();

    bool ParseLiteral(int &out, std::string &arguments);
    void WriteInt(int value);
    void WriteInt(int value, std::vector<char> &target);

    void PrintAbort(unsigned int line);

private:
    CompState m_State = CompState::INIT;

    std::vector<std::string> m_Lines;
    std::vector<char> m_Bytecode;

    unsigned int m_HeaderSize = 0;
    unsigned int m_StackSize = 1048576;
};
