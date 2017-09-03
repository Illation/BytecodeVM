#include "AssemblyCompiler.h"

#include <iostream>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <iterator>

#include "Opcode.h"

//Constructor Destructor
AssemblyCompiler::AssemblyCompiler()
{
}

AssemblyCompiler::~AssemblyCompiler()
{
    m_Lines.clear();
    m_Bytecode.clear();
}


//Input
void AssemblyCompiler::SetSource(std::vector<std::string> lines)
{
    m_Lines = lines;
    m_State = CompState::SOURCE;
}
bool AssemblyCompiler::LoadSource(std::string filename)
{
    std::ifstream file(filename);  
    if(!file.good())
    {
        std::cerr << "[ASM CMP] Input filestream could not be created" << std::endl;
        m_Lines.clear();
        m_State = CompState::INIT;
        return false;
    }

    std::string line;
    while (std::getline(file, line))
    {
        m_Lines.push_back(line);
    }

    if(m_Lines.size() <= 0)
    {
        std::cerr << "[ASM CMP] No assembly lines loaded" << std::endl;
        m_Lines.clear();
        m_State = CompState::INIT;
        return false;
    }
    std::cout << "[ASM CMP] Assembly file loaded!" << std::endl;
    m_State = CompState::SOURCE;
    return true;
}


//Compile
bool AssemblyCompiler::Compile()
{
    //Check valid input
    if(m_State == CompState::INIT)
    {
        std::cerr << "[ASM CMP] No assembly provided!" << std::endl;
        return false;
    }
    if(m_State == CompState::FAILED)
    {
        std::cerr << "[ASM CMP] Provide new source before recompiling!" << std::endl;
        return false;
    }
    if(m_State == CompState::COMPILED)
    {
        std::cerr << "[ASM CMP] No new compilation required!" << std::endl;
        return false;
    }

    //Do compilation
    for(unsigned int line = 0; line < m_Lines.size(); ++line)
    {
        //Get tokens for line
        std::string opname;
        std::string arguments;
        std::size_t firstSpace = m_Lines[line].find(' ');
        if(firstSpace!=std::string::npos)
        {
            opname = m_Lines[line].substr(0, firstSpace);
            if(firstSpace < m_Lines[line].size()-1)
                arguments = m_Lines[line].substr(firstSpace+1);
        }
        else opname = m_Lines[line];

        if(opname.size() > 0)
        {
            if(OpcodeNames.count(opname))
            {
                Opcode code = OpcodeNames[opname];
                switch(code)
                {
                    case Opcode::LITERAL:
                        {
                            if(arguments.size()==0)
                            {
                                std::cerr << "[ASM CMP] " << line << ", " << opname << ": Incorrect amount of arguments!" << std::endl;
                                std::cerr << m_Lines[line] << std::endl;
                                std::cerr << "[ASM CMP]" <<  std::endl;
                                std::cerr << "[ASM CMP] Aborting compilation!" <<  std::endl;
                                m_State = CompState::FAILED;
                                return false;
                            }
                            char parsed;
                            if(ParseChar(parsed, arguments))
                            {
                                m_Bytecode.push_back(static_cast<char>(code));
                                m_Bytecode.push_back(parsed);
                            }
                            else
                            {
                                std::cerr << m_Lines[line] << std::endl;
                                std::cerr << "[ASM CMP]" <<  std::endl;
                                std::cerr << "[ASM CMP] Aborting compilation!" <<  std::endl;
                                m_State = CompState::FAILED;
                                return false;
                            } 
                        }
                        break;

                    default:
                        m_Bytecode.push_back(static_cast<char>(code));
                        break;
                }
            }
            else
            {
                std::cerr << "[ASM CMP] " << line << ": Invalid Opcode '" << opname << "'!" << std::endl;
                std::cerr << m_Lines[line] << std::endl;
                std::cerr << "[ASM CMP]" <<  std::endl;
                std::cerr << "[ASM CMP] Aborting compilation!" <<  std::endl;
                m_State = CompState::FAILED;
                return false;
            }
        }
        else continue;
    }

    std::cout << "[ASM CMP] Compilation Complete, no errors detected!" << std::endl;
    m_State = CompState::COMPILED;
    return true;
}


//Output Results
bool AssemblyCompiler::Save(std::string filename)
{
    if(!(m_State == CompState::COMPILED))
    {
        std::cerr << "[ASM CMP] File not saved, source not compiled!" << std::endl;
        return false;
    }

    std::ofstream output( filename, std::ios::binary );
    if(!(output.good()))
    {
        std::cerr << "[ASM CMP] File not saved, file " << filename << " could not be created" << std::endl;
        return false;
    }

    for(int i = 0; i < m_Bytecode.size(); ++i)
    {
        output << m_Bytecode[i];
    }

    std::cout << "[ASM CMP] Executable saved!" << std::endl;
    return true;
}
std::vector<char> AssemblyCompiler::GetBytecode()
{
    if(!(m_State == CompState::COMPILED))
    {
        std::cerr << "[ASM CMP] Warning, non compiled bytecode accessed!" << std::endl;
    }
	return m_Bytecode;
}


//Parsing helpers
bool isNumber(const std::string& s)
{
    std::string::const_iterator it = s.begin();
    while (it != s.end() && std::isdigit(*it)) ++it;
    return !s.empty() && it == s.end();
}
bool AssemblyCompiler::ParseChar(char &out, std::string &arguments)
{
    if(arguments[0]=='\'')
    {
        out = arguments[1];
        std::size_t nDelim = arguments.find('\'', 1);
        if(nDelim = std::string::npos)
        {
            arguments = std::string();
        }
        else arguments = arguments.substr(nDelim+1);
        return true;
    }
    else if(isNumber(arguments))
    {
        std::size_t nDelim = arguments.find(' ', 1);
        if(nDelim = std::string::npos)
        {
            out = (char)stoi(arguments);
            arguments = std::string();
        }
        else
        {
            out = (char)stoi(arguments.substr(0, nDelim-1));
            arguments = arguments.substr(nDelim+1);
        }
        return true;
    }
    return false;
}
