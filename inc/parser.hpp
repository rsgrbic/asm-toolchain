#ifndef _SYNTAX_HPP
#define _SYNTAX_HPP

#include <vector>
#include <iostream>
#include <assembler.hpp>
#include <symbol_table.hpp>

#define PC 15
#define SP 14

enum SymbolType {
    SYM_SYM,
    SYM_LIT
};

struct SymbolListElement {
    SymbolType type;
    union {
        int num;
        char* sym;
    } data;
};

enum OperandType {
    OP_IMMED,
    OP_REGDIR,
    OP_REGIND,
    OP_REGINDPOM,
    OP_MEM
};

struct Operand {
    OperandType type;
    int reg;
    int num;
    char* sym;
};  

enum DirectiveType {
    DIR_GLOBAL,
    DIR_EXTERN,
    DIR_SECTION,
    DIR_WORD,
    DIR_SKIP,
    DIR_ASCII,
    DIR_END
};

enum InstructionType {
    INS_HALT,
    INS_INT,
    INS_IRET,
    INS_CALL,
    INS_RET,
    INS_JMP,
    INS_BEQ,
    INS_BNE,
    INS_BGT,
    INS_PUSH,
    INS_POP,
    INS_XCHG,
    INS_ADD,
    INS_SUB,
    INS_MUL,
    INS_DIV,
    INS_NOT,
    INS_AND,
    INS_OR,
    INS_XOR,
    INS_SHL,
    INS_SHR,
    INS_LD,
    INS_ST,
    INS_CSRRD,
    INS_CSRWR
};

class CodeComponent {
public:
    char* label;
    virtual void process()=0;
    virtual ~CodeComponent();

};

class Instruction : public CodeComponent {
public:
    InstructionType type;
    Operand operand;
    int regS;
    int regD;
    void process();
    explicit Instruction(InstructionType t) : type(t), regS(-1), regD(-1) {}

    private:
    void haltHandler();
    void intHandler();
    void iretHandler();
    void callHandler();
    void retHandler();
    void jmpHandler();
    void beqHandler();
    void bneHandler();
    void bgtHandler();
    void pushHandler();
    void popHandler();
    void xchgHandler();
    void addHandler();
    void subHandler();
    void mulHandler();
    void divHandler();
    void notHandler();
    void andHandler();
    void orHandler();
    void xorHandler();
    void shlHandler();
    void shrHandler();
    void ldHandler();
    void stHandler();
    void csrrdHandler();
    void csrwrHandler();

};

class Directive : public CodeComponent {
public:
    DirectiveType type;
    std::vector<SymbolListElement>* syms;
    char* stringValue; //SECTION NAME ; STRING
    int num;
    void process() override;

    explicit Directive(DirectiveType t) : type(t), syms(nullptr), stringValue(nullptr), num(0) {}

private:
    int globalHandler();
    int externHandler();
    int sectionHandler();
    int wordHandler();
    int skipHandler();
    int asciiHandler();
    int endHandler();

};

static void addLabel(char* label){
  Assembler& asmbl = Assembler::getInstance();
  SymbolTable& sym = SymbolTable::getInstance();
  SymbolTableEntry* entry = sym.getEntry(label);
    if (entry == nullptr) {
        sym.addEntry(label);
        entry = sym.getEntry(label);
        entry->isDefined = true;
        entry->sectionNdx = asmbl.getCurrentSectionIndex();
        entry->offset = asmbl.getCurrentSection()->getLocationCounter();\
    } else if (entry->isDefined) {
        std::cerr << "Error: label '" << label << "' already defined.\n";
    } else {
        entry->isDefined = true;
        entry->sectionNdx = asmbl.getCurrentSectionIndex();
        entry->offset = asmbl.getCurrentSection()->getLocationCounter();
        resolveForwardRefs(entry);
    }
}

class LabelComponent : public CodeComponent {
public:
    void process();

    explicit LabelComponent(char* lbl) { label = lbl; }
};

#endif
