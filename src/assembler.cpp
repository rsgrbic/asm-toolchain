#include "../inc/assembler.hpp"

Assembler* Assembler::instance = nullptr;

Assembler& Assembler::getInstance() {
    static Assembler instance;
        return instance;
}

void Assembler::initialize() {
    this->finished=false;
    sectionsList.clear();
    sectionNdxToSection.clear();
    addNewSection("UND");
    currentSectionNdx = 0;
}

int Assembler::addNewSection(const std::string& name) {
    int ndx = sectionsList.size();
    sectionsList.push_back(new Section(name, ndx));
    sectionNdxToSection[ndx] = sectionsList.back();
    return ndx;
}

Section* Assembler::getCurrentSection() const {
    return sectionNdxToSection.at(currentSectionNdx);
}

void Assembler::setCurrentSection(int ndx) {
    currentSectionNdx = ndx;
}

Section* Assembler::getSection(int ndx) {
    return sectionNdxToSection[ndx];
}

std::vector<Section*>& Assembler::getSectionsList() {
    return sectionsList;
}

void Assembler::insertLiteralPools(){
     for (auto* section : sectionsList) {
        for (auto& pair : section->getLiteralPool()) {
            int literalValue = pair.first;
            int poolLocation = section->getLocationCounter();
            section->addQuadbyte(literalValue);

            for (int offset : pair.second) {
                int disp = poolLocation - offset - 4;
                section->changeDisplacementInInstruction(offset, disp);
            }
        }
    }
}