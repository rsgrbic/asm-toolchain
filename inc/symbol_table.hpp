#ifndef _SYMBOL_TABLE_HPP
#define _SYMBOL_TABLE_HPP

#include <string>
#include <vector>
#include <unordered_map>
#include <fstream>


struct ForwardRef {
    int sectionNdx;
    int patchOffset;  // byte offset within section content
    bool isWord;      // true for .word references
    ForwardRef* next;
};

struct SymbolTableEntry {
    int index;
    std::string name;
    int offset;
    int sectionNdx;
    bool global;
    bool absolute;
    bool isDefined;
    bool isExtern;
    ForwardRef * forwardRefHead=nullptr;
};



class SymbolTable {
public:
    static SymbolTable& getInstance() {
        static SymbolTable instance;
        return instance;
    }

    void addForwardReference(const std::string& symbol, int patchOffset, int sectionNdx, bool isWord);

    int addEntry(const std::string& name) {
        SymbolTableEntry* entry = new SymbolTableEntry();
        entry->index = table.size();
        entry->name = name;
        table.push_back(entry);
        tableMap[name] = entry;
        return entry->index;
    }

    SymbolTableEntry* getEntry(const std::string& name) {
        if (tableMap.find(name) != tableMap.end())
         return tableMap[name];
      else
        return nullptr;
    }

    SymbolTableEntry* getEntry(int index) {
        return (size_t)index < table.size() ? table[index] : nullptr;
    }

    std::vector<SymbolTableEntry*>& getTable() { return table; }
    void printToOutput(std::ofstream& out);
private:
    SymbolTable() = default;
    SymbolTable(const SymbolTable&) = delete;
    void operator=(const SymbolTable&) = delete;


    std::vector<SymbolTableEntry*> table;
    std::unordered_map<std::string, SymbolTableEntry*> tableMap;
};

#endif