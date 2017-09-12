#pragma once
#include <string>
#include <vector>

#include "AtomicTypes.h"

enum class SymbolType : uint8
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
	SymbolTable(uint32 stackSize);

	bool AddFunction(const std::string &name, std::string &arguments);
	bool AddLabel(const std::string &name);
	bool AddVariable(const std::string &name, bool isArg = false);
	
	void SetParsingStatic(bool staticSection = true, std::string functionName = "");
	void AllocateStatic();

	bool HasSymbol(const std::string &name) const;
	uint32 GetValue(const std::string &name) const;
	uint32 GetFunctionArgCount(const std::string &name) const;
	uint32 GetFunctionVarCount(const std::string &name) const;

	uint32 m_NumInstructions = 0;

private:
	uint32 m_StackSize;

    struct Symbol
    {
        std::string name;
        uint32 value = 0;
		SymbolType type;
    };
    std::vector<SymbolTable::Symbol> m_Table;

	struct Func
	{
		std::string name;
		uint32 numArg = 0;
		uint32 numLoc = 0;
	};
    std::vector<SymbolTable::Func> m_FuncTable;
	Func m_CurrentFunc;

	bool m_ParsingStatic = true;
	
	//Base addresses for static / automatic memory allocation
	uint32 m_StaticCounter = 0;
};
