#include "parser.hpp"
#include "symbol_table.hpp"
#include "assembler.hpp"

void Directive::process() {

  if (this->label){
    addLabel(this->label);
  }
    switch (type) {
        case DIR_GLOBAL:  globalHandler(); break;
        case DIR_EXTERN:  externHandler(); break;
        case DIR_SECTION: sectionHandler(); break;
        case DIR_WORD:    wordHandler(); break;
        case DIR_SKIP:    skipHandler(); break;
        case DIR_ASCII:   asciiHandler(); break;
        case DIR_END:     endHandler(); break;
    }
}

int Directive::globalHandler() {
    SymbolTable &sym = SymbolTable::getInstance();
    if (!syms) {std::cerr <<"Global directive with no symbols";return 0;}
    for (auto& elem : *syms) {
        if (elem.type != SYM_SYM) continue; //SIlent fallback
        if (findSectionByName(elem.data.sym) != nullptr) {
            std::cerr << "Error: section '" << elem.data.sym
                      << "' can't be global.\n";
            return -1;
        }
        SymbolTableEntry* entry = sym.getEntry(elem.data.sym);
        if (entry == nullptr) {
            int idx = sym.addEntry(elem.data.sym);
            entry = sym.getEntry(elem.data.sym);
            entry->global = true;
        }
         else {
            entry->global = true;
        }
    }
    return 0;
}

int Directive::externHandler() {
    SymbolTable& sym = SymbolTable::getInstance();
    if (!syms) return 0;
    for (auto& elem : *syms) {
        if (elem.type != SYM_SYM) continue;// fallback
        if (findSectionByName(elem.data.sym) != nullptr) {
            std::cerr << "Error: section '" << elem.data.sym
                      << "' can't be extern.\n";
            return -1;
        }
        SymbolTableEntry* entry = sym.getEntry(elem.data.sym);
        if (entry == nullptr) {
            int idx = sym.addEntry(elem.data.sym);
            entry = sym.getEntry(elem.data.sym);
            entry->isExtern = true;
            entry->isDefined = true;
            entry->sectionNdx = 0;
        } else if (!entry->isDefined) {
            entry->isExtern = true;
            entry->isDefined = true;
            entry->sectionNdx = 0;
            resolveForwardRefs(entry);
        } else {
            std::cerr << "Error: symbol " << elem.data.sym << " already defined." << std::endl;
            return -1;
        }
    }
    return 0;
}

int Directive::wordHandler() {
    SymbolTable& sym = SymbolTable::getInstance();
    Assembler& asmbl = Assembler::getInstance();
    Section* sec = asmbl.getCurrentSection();

    for (auto& elem : *syms) {
        if (elem.type == SYM_LIT) {
            sec->addQuadbyte(elem.data.num);
        } else {
            SymbolTableEntry* entry = sym.getEntry(elem.data.sym);
            if (!entry) {
                sym.addEntry(elem.data.sym);
                entry = sym.getEntry(elem.data.sym);
            }
            if (!entry->isDefined) {
                // Forward reference: record patch location, emit 0 placeholder
                sym.addForwardReference(entry->name, sec->getLocationCounter(), sec->getNdx(), true);
                sec->addQuadbyte(0);
            } else {
                // Already defined: emit value + relocation
                sec->getRelocationTable().addEntry(sec->getNdx(), sec->getLocationCounter(), entry->index);
                sec->addQuadbyte(entry->offset);
            }
        }
    }
    return 0;
}

int Directive::skipHandler() {
    Assembler& asmbl = Assembler::getInstance();
    for (int i = 0; i < num; i++)
        asmbl.getCurrentSection()->addByte(0);
    return 0;
}

int Directive::asciiHandler() {
    Assembler& asmbl = Assembler::getInstance();
    Section* sec = asmbl.getCurrentSection();
    for (char* p = stringValue; *p; p++)
        sec->addByte(*p);
    sec->addByte(0);
    return 0;
}
int Directive::endHandler() {
    Assembler::getInstance().setFinished(true);
    return 0;
}

int Directive::sectionHandler() {
    SymbolTable& sym = SymbolTable::getInstance();
    Assembler& asmbl = Assembler::getInstance();

    if (findSectionByName(stringValue) != nullptr) {
            std::cerr << "Error: section '" << stringValue
                      << "' can't be extern.\n";
            return -1;
    }

    SymbolTableEntry* entry = sym.getEntry(stringValue);
     if (entry != nullptr && entry->isDefined) {
        std::cerr << "Error: name '" << stringValue
                  << "' already used as a symbol.\n";
        return -1;
    }

    int ndx = asmbl.addNewSection(stringValue);
    asmbl.setCurrentSection(ndx);
    return 0;
}
