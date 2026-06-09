#ifndef _ASSEMBLER_HPP
#define _ASSEMBLER_HPP

#include <vector>
#include <unordered_map>
#include "section.hpp"

class Assembler {
    static Assembler* instance;
    std::vector<Section*> sectionsList;
    std::unordered_map<int, Section*> sectionNdxToSection;
    int currentSectionNdx;
    bool finished;
    Assembler() : currentSectionNdx(0) {}
public:
    static Assembler& getInstance();

    void initialize();  // creates UND section (index 0)
    bool isFinished() const {return finished;}
    void setFinished(bool v) {finished=v;}
    Section* getCurrentSection() const;
    void setCurrentSection(int ndx);

    void insertLiteralPools();

    int addNewSection(const std::string& name);
    Section* getSection(int ndx);
    std::vector<Section*>& getSectionsList();

    int getCurrentSectionIndex() const { return currentSectionNdx; }

    static bool valueWithin12BitRange(int value) {
    return value >= -2048 && value <= 2047;
}
};

#endif