#include "AssemblyCompiler.h"

#include <iostream>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <iterator>
#include <cctype>

#include "Opcode.h"
#include "SymbolTable.h"

//Constructor Destructor
AssemblyCompiler::AssemblyCompiler()
{
}

AssemblyCompiler::~AssemblyCompiler()
{
    m_Lines.clear();
    m_Bytecode.clear();

	delete m_pSymbolTable;
	m_pSymbolTable = nullptr;
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
    if(!CompileHeader())return false;
    if(!BuildSymbolTable())return false;
    if(!CompileInstructions())return false;

    std::cout << "[ASM CMP] Compilation Complete, no errors detected!" << std::endl;
    m_State = CompState::COMPILED;
    return true;
}

bool AssemblyCompiler::BuildSymbolTable()
{
	if(!m_pSymbolTable)
		m_pSymbolTable = new SymbolTable(m_StackSize);
	else
		std::cerr << "[ASM CMP] Symbol table already created!" << std::endl;

	for(uint32 line = 0; line < m_Lines.size(); ++line)
    {
        std::string opname;
        std::string arguments;
        if(!TokenizeLine(m_Lines[line], opname, arguments))continue;

		//Jump labels
        if(opname[0] == '@')
        {
            if(!m_pSymbolTable->AddLabel(opname))
            {
                std::cerr << "[ASM CMP] " << line << ": label " << opname << " already defined!" << std::endl;
                return false;
            }
			continue;
        }
        if(opname[0] == '$')
        {
			m_pSymbolTable->m_NumInstructions += 4; //First instruction is int32 numArgs
            if(!m_pSymbolTable->AddFunction(opname, arguments))
            {
                std::cerr << "[ASM CMP] " << line << ": error adding function: " << opname << std::endl;
                return false;
            }
			continue;
        }

        if(!IsValidOpname(opname, line))return false;
        Opcode code = OpcodeNames[opname];

        switch(code)
        {
        case Opcode::LITERAL:
            {
                m_pSymbolTable->m_NumInstructions += 5; 
                if(!HasValidArgs(arguments, line, opname))return false;
                CheckVar(arguments);
            }
            break;

        case Opcode::LITERAL_ARRAY:
            {
                if(!HasValidArgs(arguments, line, opname))return false;

                if(arguments[0] == '\"')
                {
                    if(arguments.size()==1)
                    {
                        std::cerr << "[ASM CMP] " << line << ", " << opname << ": Incorrect argument size!" << std::endl;
                        PrintAbort(line);
                        return false;
                    }
                    uint32 j = 1;
                    while(arguments[j] != '\"')
                    {
                        m_pSymbolTable->m_NumInstructions+=4;
                        ++j;
                    }
                }
                else
                {
                    while(arguments.size() > 0)
                    {
                        m_pSymbolTable->m_NumInstructions+=4;
                        CheckVar(arguments);
                    }
                }
                m_pSymbolTable->m_NumInstructions+=5;
            }
            break;

        default:
            m_pSymbolTable->m_NumInstructions++;
            break;
        }
    }

	m_pSymbolTable->AllocateStatic();

    return true;
}

bool AssemblyCompiler::CompileInstructions()
{
    for(uint32 line = 0; line < m_Lines.size(); ++line)
    {
        std::string opname;
        std::string arguments;
        if(!TokenizeLine(m_Lines[line], opname, arguments))continue;
        if(opname[0] == '@') continue; //Skip labels
        if(opname[0] == '$') //Write num arguments for function
        {
			int32 numArguments = 0;
			while(arguments.size() > 0)
			{
				std::size_t nDelim = arguments.find('\'', 1);
				if(nDelim == std::string::npos) break;
				arguments = arguments.substr(nDelim+1);
				++numArguments;
			}
			WriteInt(numArguments);
			continue;
        }

        if(!IsValidOpname(opname, line))return false;

        Opcode code = OpcodeNames[opname];

        switch(code)
        {
        case Opcode::LITERAL:
            {
                if(!HasValidArgs(arguments, line, opname))return false;
                int32 parsed;
                if(ParseLiteral(parsed, arguments))
                {
                    m_Bytecode.push_back(static_cast<uint8>(code));
                    WriteInt(parsed);
                }
                else
                {
                    PrintAbort(line);
                    return false;
                } 
            }
            break;

        case Opcode::LITERAL_ARRAY:
            {
                if(!HasValidArgs(arguments, line, opname))return false;
                m_Bytecode.push_back(static_cast<uint8>(code));

                std::vector<int32> arr;
                if(arguments[0] == '\"')
                {
                    if(arguments.size()==1)
                    {
                        std::cerr << "[ASM CMP] " << line << ", " << opname << ": Incorrect argument size!" << std::endl;
                        PrintAbort(line);
                        return false;
                    }
                    uint32 j = 1;
                    while(arguments[j] != '\"')
                    {
                        if(j >= arguments.size())
                        {
                            std::cerr << "[ASM CMP] " << line << ", " << opname << ": Expected ' \" ' !" << std::endl;
                            PrintAbort(line);
                            return false;
                        }
                        arr.push_back(int32(arguments[j]));

                        ++j;
                    }
                }
                else
                {
                    while(arguments.size() > 0)
                    {
                        int32 parsed;
                        if(ParseLiteral(parsed, arguments))
                        {
                            WriteInt(parsed);
                            arr.push_back(parsed);
                        }
                        else
                        {
                            PrintAbort(line);
                            return false;
                        } 
                    }
                }
                WriteInt(static_cast<int32>(arr.size()));
                for(auto value : arr) WriteInt(value);
            }
            break;

        default:
            m_Bytecode.push_back(static_cast<uint8>(code));
            break;
        }
    }
    return true;
}

bool AssemblyCompiler::CompileHeader()
{
    std::vector<uint8> header;
    WriteInt(m_StackSize, header);

    m_HeaderSize = header.size();
    m_Bytecode.insert(m_Bytecode.end(), header.begin(), header.end());
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

    for(uint32 i = 0; i < m_Bytecode.size(); ++i)
    {
        output << m_Bytecode[i];
    }

    std::cout << "[ASM CMP] Executable saved!" << std::endl;
    return true;
}
std::vector<uint8> AssemblyCompiler::GetBytecode()
{
    if(!(m_State == CompState::COMPILED))
    {
        std::cerr << "[ASM CMP] Warning, non compiled bytecode accessed!" << std::endl;
    }
	return m_Bytecode;
}


//Parsing helpers
bool AssemblyCompiler::TokenizeLine(std::string line, std::string &opname, std::string &arguments)
{
    //Comment
    if(line.size() > 1 && line[0] == '/' && line[1] == '/')
        return false;
    //Empty Line
    if(line.size() == 0)
        return false;
    //get op and arguments
    std::size_t firstSpace = line.find(' ');
    if(firstSpace!=std::string::npos)
    {
        opname = line.substr(0, firstSpace);
        if(firstSpace < line.size()-1)
            arguments = line.substr(firstSpace+1);
    }
    else opname = line;

    return true;
}
bool AssemblyCompiler::IsValidOpname(std::string opname, uint32 line)
{
    if(!(OpcodeNames.count(opname)))
    {
        std::cerr << "[ASM CMP] " << line << ": Invalid Opcode '" << opname << "'!" << std::endl;
        PrintAbort(line);
        return false;
    }
    return true;
}

void AssemblyCompiler::CheckVar(std::string &arguments)
{
    //Separate first argument out
    std::string arg;
    std::size_t nDelim = arguments.find(' ', 1);
    if(nDelim == std::string::npos)
    {
        arg = arguments;
        arguments = std::string();
    }
    else
    {
        arg = arguments.substr(0, nDelim);
        arguments = arguments.substr(nDelim+1);
    }
    //Handle found variable
    if(arg[0]=='#')
    {
		m_pSymbolTable->AddVariable(arg);
    }
}

bool AssemblyCompiler::HasValidArgs(std::string arguments, uint32 line, std::string opname)
{
    if(arguments.size()==0)
    {
        std::cerr <<"[ASM CMP] " << line << ", " << opname << ": Incorrect amount of arguments!" << std::endl;
        PrintAbort(line);
        return false;
    }
    return true;
}
bool isNumber(const std::string& s)
{
    std::string::const_iterator it = s.begin();
    while (it != s.end() && std::isdigit(*it)) ++it;
    return !s.empty() && it == s.end();
}
bool AssemblyCompiler::ParseLiteral(int32 &out, std::string &arguments)
{
    if((arguments[0]=='#') || (arguments[0]=='@') || (arguments[0]=='$')) //Replace mnemonics (variables, lables, functions)
    {
        std::string arg;
        std::size_t nDelim = arguments.find(' ', 1);
        if(nDelim == std::string::npos)
        {
            arg = arguments;
            arguments = std::string();
        }
        else
        {
            arg = arguments.substr(0, nDelim);
            arguments = arguments.substr(nDelim+1);
        }
		if(m_pSymbolTable->HasSymbol(arg)) 
		{
			out = static_cast<int32>(m_pSymbolTable->GetValue(arg));
			return true;
		}
        std::cerr << "[ASM CMP] Couldn't find symbol: " << arg << std::endl;
    }
    else if(arguments[0]=='\'') //char
    {
        out = int32(arguments[1]);
        std::size_t nDelim = arguments.find('\'', 1);
        if(nDelim == std::string::npos)
        {
            arguments = std::string();
        }
        else arguments = arguments.substr(nDelim+1);
        return true;
    }
    else if(isNumber(arguments)) //int
    {
        std::size_t nDelim = arguments.find(' ', 1);
        if(nDelim == std::string::npos)
        {
            out = stoi(arguments);
            arguments = std::string();
        }
        else
        {
            out = stoi(arguments.substr(0, nDelim));
            arguments = arguments.substr(nDelim+1);
        }
        return true;
    }
    return false;
}
void AssemblyCompiler::WriteInt(int32 value)
{
    WriteInt(value, m_Bytecode);
}
void AssemblyCompiler::WriteInt(int32 value, std::vector<uint8> &target)
{
#ifdef WORD_BIG_ENDIAN
   target.push_back(value & 0xFF);
   target.push_back((value >> 8) & 0xFF);
   target.push_back((value >> 16) & 0xFF);
   target.push_back((value >> 24) & 0xFF);
#elif
   target.push_back((value >> 24) & 0xFF);
   target.push_back((value >> 16) & 0xFF);
   target.push_back((value >> 8) & 0xFF);
   target.push_back(value & 0xFF);
#endif
}

void AssemblyCompiler::PrintAbort(uint32 line)
{
    std::cerr << m_Lines[line] << std::endl;
    std::cerr << "[ASM CMP]" <<  std::endl;
    std::cerr << "[ASM CMP] Aborting compilation!" <<  std::endl;
    m_State = CompState::FAILED;
}
