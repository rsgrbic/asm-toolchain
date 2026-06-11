#include "../inc/assembler.hpp"
#include "inc/symbol_table.hpp"
#include <iostream>
#include <fstream>
#include "assembler.hpp"
Assembler* Assembler::instance = nullptr;

Assembler& Assembler::getInstance() {
    static Assembler instance;
        return instance;
}

void Assembler::initialize() {
    this->finished=false;
    sectionsList.clear();
    sectionNdxToSection.clear();
    sectionNameToSection.clear();
    addNewSection("UND");
    currentSectionNdx = 0;
}

int Assembler::addNewSection(const std::string& name) {
    int ndx = sectionsList.size();
    sectionsList.push_back(new Section(name, ndx));
    sectionNdxToSection[ndx] = sectionsList.back();
    sectionNameToSection[name]=sectionsList.back();
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

Section* Assembler::getSectionByName(const std::string& name){
    auto it = sectionNameToSection.find(name);
    return (it !=sectionNameToSection.end()?it->second:nullptr);
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

            for (int patchLocation : pair.second) {
                int disp = poolLocation - patchLocation - 4;
                section->changeDisplacementInInstruction(patchLocation, disp);
            }
        }
    }
}

void Assembler::insertSymbolPools() {
    SymbolTable& sym = SymbolTable::getInstance();
    for (auto* section : sectionsList) {
        for (auto& pair : section->getsymbolPool()) {
            SymbolTableEntry* entry = sym.getEntry(pair.first);
            int appendLocation = section->getLocationCounter();

            // Append 4-byte symbol value (0 for undefined/extern — linker fills)
            section->addQuadbyte(entry->isDefined ? entry->offset : 0);
            section->getRelocationTable().addEntry(section->getNdx(),appendLocation,entry->index);

            for (int patchLocation : pair.second) {
                int disp = appendLocation - (patchLocation + 4);
                section->changeDisplacementInInstruction(patchLocation,
                    static_cast<uint32_t>(disp & 0xFFF));
            }
        }
    }
}


void resolveForwardRefs(SymbolTableEntry* entry) {
    Assembler& asmbl = Assembler::getInstance();
    ForwardRef* ref = entry->forwardRefHead;
    while (ref) {
        Section* sec = asmbl.getSection(ref->sectionNdx);
        if (ref->isWord) {
            // .word: patch 4 bytes directly in-place
            sec->addQuadbyteAtOffset(entry->offset, ref->patchOffset);
            sec->getRelocationTable().addEntry(
                sec->getNdx(), ref->patchOffset, entry->index);
        } else {
            // instruction: defer to pool-based resolution
            sec->addSymbolToPool(entry->name);
            // note: the instruction already has disp=0 at ref->patchOffset,
            // insertSymbolPools will patch it later
        }
        ForwardRef* next = ref->next;
        delete ref;
        ref = next;
    }
    entry->forwardRefHead = nullptr;
}

bool Assembler::checkIfAllSymbolsAreDefined() {
    SymbolTable& sym = SymbolTable::getInstance();
    bool ok = true;
    for (auto* e : sym.getTable()) {
        // Acceptable if defined, OR if declared global/extern
        if (e->isDefined || e->global || e->isExtern) continue;
        std::cerr << "Error: Symbol '" << e->name 
                  << "' is referenced but not defined.\n";
        ok = false;
    }
    return ok;
}


void Assembler::promoteGlobalsToExtern() {
    SymbolTable& sym = SymbolTable::getInstance();
    for (auto* e : sym.getTable()) {
        // A global symbol that was never defined locally becomes extern-like
        if (e->global && !e->isDefined) {
            e->isExtern = true;
            e->isDefined = true;
            e->sectionNdx = 0;
            e->offset = 0;
        }
    }
}
void Assembler::writeObjectFile(const std::string &filename)
{
    std::ofstream out(filename);
    if (!out) {
        std::cerr << "Error: cannot open " << filename << "\n";
        return;
    }
    out << "# SYMT: index name value section_ndx is_defined is_extern is_global\n"
        << "# SECTION: name index size\n"
        << "# RELT: section_ndx offset symbol_index\n";
    SymbolTable::getInstance().printToOutput(out);
    out << "\n";
    for (auto* sec : sectionsList) {
        if (sec->getName() == "UND") continue;
        sec->printToOutput(out);
        out << "\n";
    }
    out.close();
}