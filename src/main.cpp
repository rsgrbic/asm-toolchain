#include <iostream>
#include <cstdio>
#include <vector>

#include "parser.hpp"
#include "parser.tab.hpp"
#include "symbol_table.hpp"
#include "assembler.hpp"

extern int yyparse();
extern FILE* yyin;
extern unsigned lineNum;
extern void yylex_destroy();
void printComponents(const std::vector<CodeComponent*>& components) {
    for (size_t i = 0; i < components.size(); i++) {
        auto* c = components[i];
        std::cout << "[" << i << "] ";
        if (c->label) std::cout << "label='" << c->label << "' ";

        if (auto* ins = dynamic_cast<Instruction*>(c)) {
            static const char* names[] = {
                "halt","int","iret","call","ret","jmp",
                "beq","bne","bgt","push","pop","xchg",
                "add","sub","mul","div","not","and","or","xor",
                "shl","shr","ld","st","csrrd","csrwr"
            };
            std::cout << "INSTRUCTION " << names[ins->type];
            std::cout << " regD=" << ins->regD << " regS=" << ins->regS;
            std::cout << " operand(type=" << ins->operand.type
                      << " reg=" << ins->operand.reg
                      << " num=" << ins->operand.num
                      << " sym=" << (ins->operand.sym ? ins->operand.sym : "null") << ")";
        }
        else if (auto* d = dynamic_cast<Directive*>(c)) {
            static const char* names[] = {
                ".global",".extern",".section",".word",
                ".skip",".ascii",".end"
            };
            std::cout << "DIRECTIVE " << names[d->type];
            if (d->stringValue) std::cout << " sym='" << d->stringValue << "'";
            if (d->num) std::cout << " num=" << d->num;
            if (d->syms) {
                std::cout << " syms=[";
                for (auto& e : *d->syms)
                    std::cout << (e.type == SYM_SYM ? e.data.sym : std::to_string(e.data.num)) << " ";
                std::cout << "]";
            }
        }
        else {
            std::cout << "LABEL (label-only line)";
        }
        std::cout << std::endl;
    }
}

void printSymbolTable() {
    auto& sym = SymbolTable::getInstance();
    std::cout << "\n=== SYMBOL TABLE ===" << std::endl;
    for (auto* e : sym.getTable()) {
        std::cout << "  #" << e->index << " '" << e->name
                  << "' val=" << e->offset
                  << " sec=" << e->sectionNdx
                  << " def=" << e->isDefined
                  << " glb=" << e->global
                  << " ext=" << e->isExtern
                  << " secSym=" << e->isSection
                  << std::endl;
    }
}


void printSections() {

    auto& asmbl = Assembler::getInstance();
    std::cout << "\n=== SECTIONS ===" << std::endl;
    for (auto* sec : asmbl.getSectionsList()) {
        std::cout << "  '" << sec->getName() << "' ndx=" << sec->getNdx()
                  << " LC=" << sec->getLocationCounter() << " bytes=[";
        for (auto b : sec->getContent())
            std::cout << std::hex << (int)b << " ";
        std::cout << std::dec << "]" << std::endl;
    }
}

int main(int argc, char** argv) {
    if (argc != 2) {
        std::cerr << "Usage: " << argv[0] << " <input.s>" << std::endl;
        return 1;
    }

    yyin = fopen(argv[1], "r");
    if (!yyin) { perror("fopen"); return 1; }

    // Init state
    lineNum = 1;
    SymbolTable::getInstance().addEntry("UND");
    Assembler::getInstance().initialize();

    // Parse
    std::vector<CodeComponent*> components;
    int result = yyparse(components);
    printComponents(components);

    if (result != 0) {
        std::cerr << "Parsing failed." << std::endl;
        return result;
    }

    // Process — one pass
    for (auto* c : components){
        c->process();
        if(Assembler::getInstance().isFinished()) break;
    }
    Assembler::getInstance().insertLiteralPools();

    std::cout << "Assembling successful (" << components.size() << " components)." << std::endl;
    printSymbolTable();
    printSections();

    // Cleanup
    for (auto* c : components) delete c;
    fclose(yyin);
    yylex_destroy();
    return 0;
}



void lol();