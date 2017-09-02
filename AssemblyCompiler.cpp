#include "AssemblyCompiler.h"

#include <iostream>
#include <fstream>
#include <algorithm>
#include <iterator>

#include "Opcode.h"

AssemblyCompiler::AssemblyCompiler(std::string filename)
{
    std::ifstream file(filename);  
    std::copy(std::istream_iterator<std::string>(file), 
            std::istream_iterator<std::string>(), std::back_inserter(m_Lines));
}
AssemblyCompiler::~AssemblyCompiler()
{
    m_Lines.clear();
}

bool AssemblyCompiler::Compile(std::string filename)
{
    std::ofstream output( filename, std::ios::binary );

    return true;
}
