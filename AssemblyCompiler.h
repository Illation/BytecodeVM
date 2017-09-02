#pragma once

#include <string>
#include <vector>

class AssemblyCompiler
{
public:
    AssemblyCompiler(std::string filename);
    ~AssemblyCompiler();

    bool Compile(std::string filename);

private:
    std::vector<std::string> m_Lines;
};
