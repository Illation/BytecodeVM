#include "Opcode.h"

std::string GetOpString(Opcode code)
{
    std::string key = "invalid code";
    for (auto &i : OpcodeNames) 
    {
        if (i.second == code) 
        {
            key = i.first;
            break; // to stop searching
        }
    }
	return key;
}
