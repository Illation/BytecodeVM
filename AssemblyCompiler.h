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
    bool ParseChar(char &out, std::string &arguments);
    void WriteInt(int value);

    void PrintAbort(unsigned int line);

private:
    CompState m_State = CompState::INIT;

    std::vector<std::string> m_Lines;
    std::vector<char> m_Bytecode;
};
