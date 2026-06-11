#ifndef _ASSEMBLER_HPP
#define _ASSEMBLER_HPP

#include <vector>
#include <unordered_map>
#include "section.hpp"

struct SymbolTableEntry;

void resolveForwardRefs(SymbolTableEntry* entry);

class Assembler {
    static Assembler* instance;
    std::vector<Section*> sectionsList;
    std::unordered_map<int, Section*> sectionNdxToSection;
    std::unordered_map<std::string, Section*> sectionNameToSection;

    int currentSectionNdx;
    bool finished;
    Assembler() : currentSectionNdx(0) {}
public:
    static Assembler& getInstance();
    Section* getSectionByName(const std::string& name);
    void initialize();  // creates UND section (index 0)
    bool isFinished() const {return finished;}
    void setFinished(bool v) {finished=v;}
    Section* getCurrentSection() const;
    void setCurrentSection(int ndx);

    void insertLiteralPools();
    void insertSymbolPools();
    
    bool checkIfAllSymbolsAreDefined();
    void promoteGlobalsToExtern();
    void writeObjectFile(const std::string& filename);

    int addNewSection(const std::string& name);
    Section* getSection(int ndx);
    std::vector<Section*>& getSectionsList();

    int getCurrentSectionIndex() const { return currentSectionNdx; }
    static bool valueWithin12BitRange(int value) {
    return value >= -2048 && value <= 2047;
}
};

inline Section* findSectionByName(const std::string& name) {
    return Assembler::getInstance().getSectionByName(name);
}

#endif