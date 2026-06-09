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
    if (!syms) return 0;
    for (auto& elem : *syms) {
        if (elem.type != SYM_SYM) continue; //SIlent fallback
        SymbolTableEntry* entry = sym.getEntry(elem.data.sym);
        if (entry == nullptr) {
            int idx = sym.addEntry(elem.data.sym);
            entry = sym.getEntry(elem.data.sym);
            entry->global = true;
        } else if (entry->isSection) {
            std::cerr << "Error: section " << elem.data.sym << " can't be global." << std::endl;
            return -1;
        } else {
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
            // TODO: backpatch forward references to this symbol
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
    if (!syms) return 0;
    for (auto& elem : *syms) {
        if (elem.type == SYM_LIT) {
            asmbl.getCurrentSection()->addQuadbyte(elem.data.num);
        } else {
            SymbolTableEntry* entry = sym.getEntry(elem.data.sym);
            if (entry == nullptr) {
                int idx = sym.addEntry(elem.data.sym);
                entry = sym.getEntry(elem.data.sym);
            }
            if (!entry->isDefined) {
                // TODO: forward reference — add placeholder + forward ref node
                asmbl.getCurrentSection()->addQuadbyte(0);
            } else {
                // TODO: add relocation entry + symbol value
                asmbl.getCurrentSection()->addQuadbyte(entry->offset);
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

    SymbolTableEntry* entry = sym.getEntry(stringValue);
    if (entry == nullptr) {
        int idx = sym.addEntry(stringValue);
        entry = sym.getEntry(stringValue);
        entry->isSection = true;
        entry->isDefined = true;
        entry->offset = 0;
        entry->sectionNdx = asmbl.addNewSection(stringValue);
        asmbl.setCurrentSection(entry->sectionNdx);
    } else if (entry->isDefined) {
        std::cerr << "Error: section " << stringValue << " already defined." << std::endl;
        return -1;
    } else {
        entry->isSection = true;
        entry->isDefined = true;
        entry->offset = 0;
        entry->sectionNdx = asmbl.addNewSection(stringValue);
        asmbl.setCurrentSection(entry->sectionNdx);
        // TODO: backpatch forward references
    }
    return 0;
}
