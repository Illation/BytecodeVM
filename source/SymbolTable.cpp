#include "SymbolTable.h"

#include <cassert>
#include <iostream>

bool SymbolTable::AddFunction(const std::string &name, std::string &arguments)
{
	if(HasSymbol(name))
		return false;

	//Add Symbol
	auto sbl = SymbolTable::Symbol();
	sbl.name = name;
	sbl.value = m_StackSize + m_NumInstructions;
	sbl.type = SymbolType::FUNCTION;
	m_Table.push_back(sbl);

	//Reset counters for automatic memory allocation (non static)
	m_ArgCounter = 0;
	m_LocalCounter = 0;
	//The following variables are not static anymore
	SetParsingStatic(false);
	
	//Also add function arguments (parameters)
	while(arguments.size() > 0)
	{
		std::string arg;
		std::size_t nDelim = arguments.find(' ', 1);
		if(nDelim = std::string::npos)
		{
			arg = arguments;
			arguments = std::string();
		}
		else
		{
			arg = arguments.substr(0, nDelim-1);
			arguments = arguments.substr(nDelim+1);
		}
		if(!(AddVariable(arg, true))) return false;
	}
	return true;
}

bool SymbolTable::AddLabel(const std::string &name)
{
	if(HasSymbol(name))
		return false;
	auto sbl = SymbolTable::Symbol();
	sbl.name = name;
	sbl.value = m_StackSize + m_NumInstructions;
	sbl.type = SymbolType::LABEL;
	m_Table.push_back(sbl);
	return true;
}

bool SymbolTable::AddVariable(const std::string &name, bool isArg)
{
	if(HasSymbol(name))
		return false;
	auto sbl = SymbolTable::Symbol();
	sbl.name = name;

	//Check variable type
	assert(isArg != m_ParsingStatic);//Static segments don't have arguments

	if(isArg) sbl.type = SymbolType::ARG;
	else if(m_ParsingStatic) sbl.type = SymbolType::STATIC;
	else sbl.type = SymbolType::LOCAL;
	
	switch(sbl.type)
	{
		case SymbolType::STATIC:
			sbl.value = m_StaticCounter;
			m_StaticCounter += 4; //Multitype support should use variable size here
			break;
		case SymbolType::LOCAL:
			sbl.value = m_LocalCounter;
			m_LocalCounter += 4; 
			break;
		case SymbolType::ARG:
			sbl.value = m_ArgCounter;
			m_ArgCounter += 4; 
			break;
	}
	m_Table.push_back(sbl);
	return true;
}

void SymbolTable::AllocateStatic()
{
    std::cout << "[SYMBOL] Instruction count: " << m_NumInstructions << "; Symbols: " << std::endl;
    uint32 staticBase = m_StackSize + m_NumInstructions;
    for(uint32 i = 0; i < m_Table.size(); ++i)
    {
        if(m_Table[i].type == SymbolType::STATIC)
        {
            m_Table[i].value += staticBase;
        }
        std::cout << "[SYMBOL] name: " << m_Table[i].name << "; value: " << m_Table[i].value << std::endl;
    }
}

bool SymbolTable::HasSymbol(const std::string &name) const
{
	for(auto sbl : m_Table)
	{
		if(sbl.name == name) return true;
	}
	return false;
}

uint32 SymbolTable::GetValue(const std::string &name) const
{
	for(auto sbl : m_Table)
	{
		if(sbl.name == name) return sbl.value;
	}
	std::cerr << "[SYMBOL] Could not find Symbol " << name << std::endl;
	return 0;
}
