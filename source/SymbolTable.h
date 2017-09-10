#pragma once
#include <string>
#include <vector>

enum class SymbolType : unsigned char
{
	FUNCTION,
	LABEL,
	STATIC,
	LOCAL,
	ARG
};

class SymbolTable
{
public:
	SymbolTable(unsigned int stackSize)
		:m_StackSize(stackSize)
	{}

	bool AddFunction(const std::string &name, std::string &arguments);
	bool AddLabel(const std::string &name);
	bool AddVariable(const std::string &name, bool isArg = false);
	
	void SetParsingStatic(bool staticSection=true){m_ParsingStatic = staticSection;}
	void AllocateStatic();

	bool HasSymbol(const std::string &name) const;
	unsigned int GetValue(const std::string &name) const;

	unsigned int m_NumInstructions = 0;
private:
    struct Symbol
    {
        std::string name;
        unsigned int value = 0;
		SymbolType type;
    };
    std::vector<AssemblyCompiler::Symbol> m_Table;

	unsigned int m_StackSize;

	bool m_ParsingStatic = true;
	
	//Base addresses for static / automatic memory allocation
	unsigned int m_StaticCounter = 0;
	unsigned int m_ArgCounter = 0;
	unsigned int m_LocalCounter = 0;
};
