#include "./inc/parser.hpp"
#include "./inc/assembler.hpp"
#include "./inc/symbol_table.hpp"
#include <iostream>

static Section* sec() { return Assembler::getInstance().getCurrentSection(); }

CodeComponent::~CodeComponent() = default;

// === Dispatch ===
void Instruction::process() {
    switch (type) {
        case INS_HALT:  haltHandler();  break;
        case INS_INT:   intHandler();   break;
        case INS_IRET:  iretHandler();  break;
        case INS_CALL:  callHandler();  break;
        case INS_RET:   retHandler();   break;
        case INS_JMP:   jmpHandler();   break;
        case INS_BEQ:   beqHandler();   break;
        case INS_BNE:   bneHandler();   break;
        case INS_BGT:   bgtHandler();   break;
        case INS_PUSH:  pushHandler();  break;
        case INS_POP:   popHandler();   break;
        case INS_XCHG:  xchgHandler();  break;
        case INS_ADD:   addHandler();   break;
        case INS_SUB:   subHandler();   break;
        case INS_MUL:   mulHandler();   break;
        case INS_DIV:   divHandler();   break;
        case INS_NOT:   notHandler();   break;
        case INS_AND:   andHandler();   break;
        case INS_OR:    orHandler();    break;
        case INS_XOR:   xorHandler();   break;
        case INS_SHL:   shlHandler();   break;
        case INS_SHR:   shrHandler();   break;
        case INS_LD:    ldHandler();    break;
        case INS_ST:    stHandler();    break;
        case INS_CSRRD: csrrdHandler(); break;
        case INS_CSRWR: csrwrHandler(); break;
    }
}

void Instruction::haltHandler()  { sec()->formInstruction(HALT, 0, 0, 0, 0, 0); }
void Instruction::intHandler()   { sec()->formInstruction(INT,  0, 0, 0, 0, 0); }

void Instruction::iretHandler() {
    sec()->formInstruction(LOAD, LOAD_POP_MOD, PC, SP, 0, 4);
    sec()->formInstruction(LOAD, LOAD_CSRPOP_MOD, 0, SP, 0, 4);// 0 status
}

void Instruction::retHandler()   { sec()->formInstruction(LOAD, LOAD_POP_MOD, PC, SP, 0, 4); }

//Jumps

void Instruction::callHandler() {
    if (operand.sym == nullptr)
        sec()->formInstructionWithLiteral(CALL, CALL_REG_MOD, PC, 0, 0, operand.num);
    else{ 
       SymbolTable& sym= SymbolTable::getInstance();
       SymbolTableEntry* e = sym.getEntry(operand.sym);
       if(!e){
        sym.addEntry(operand.sym);
        e=sym.getEntry(operand.sym);
       }
       if (e->isDefined){
        sec()->addSymbolToPool(operand.sym);
    }
    else{
        //forward ref
        sym.addForwardReference(operand.sym,sec()->getLocationCounter(),sec()->getNdx(),false);
    }
    sec()->formInstruction(CALL,CALL_MEM_MOD,PC,0,0,0);
    }
}
void Instruction::jmpHandler() {
    if (operand.sym == nullptr)
        sec()->formInstructionWithLiteral(JMP, JMP_REG_MOD, PC, 0, 0, operand.num);
    else
    {
        SymbolTable& sym = SymbolTable::getInstance();
        SymbolTableEntry* e = sym.getEntry(operand.sym);
        if (!e) { sym.addEntry(operand.sym); e = sym.getEntry(operand.sym); }
        if (e->isDefined) {
            sec()->addSymbolToPool(operand.sym);
        } else {
            sym.addForwardReference(operand.sym,
                sec()->getLocationCounter(), sec()->getNdx(), false);
        }
        sec()->formInstruction(JMP, JMP_MEM_MOD, PC, 0, 0, 0);
    }
        
}

void Instruction::beqHandler() {
    if (operand.sym == nullptr)
        sec()->formInstructionWithLiteral(JMP, JMP_BEQ_MOD, PC, regD, regS, operand.num);
    else
        {
        SymbolTable& sym = SymbolTable::getInstance();
        SymbolTableEntry* e = sym.getEntry(operand.sym);
        if (!e) { sym.addEntry(operand.sym); e = sym.getEntry(operand.sym); }
        if (e->isDefined) {
            sec()->addSymbolToPool(operand.sym);
        } else {
            sym.addForwardReference(operand.sym,
                sec()->getLocationCounter(), sec()->getNdx(), false);
        }
        sec()->formInstruction(JMP, JMP_BEQ_MOD | JMP_MEM_MOD, PC, regD, regS, 0);
        }
}


void Instruction::bneHandler() {
    if (operand.sym == nullptr)
        sec()->formInstructionWithLiteral(JMP, JMP_BNE_MOD, PC, regD, regS, operand.num);
    else
        {
        SymbolTable& sym = SymbolTable::getInstance();
        SymbolTableEntry* e = sym.getEntry(operand.sym);
        if (!e) { sym.addEntry(operand.sym); e = sym.getEntry(operand.sym); }
        if (e->isDefined) {
            sec()->addSymbolToPool(operand.sym);
        } else {
            sym.addForwardReference(operand.sym,
                sec()->getLocationCounter(), sec()->getNdx(), false);
        }
        sec()->formInstruction(JMP, JMP_BNE_MOD | JMP_MEM_MOD, PC, regD, regS, 0);
    }
}

void Instruction::bgtHandler() {
    if (operand.sym == nullptr)
        sec()->formInstructionWithLiteral(JMP, JMP_BGT_MOD, PC, regD, regS, operand.num);
    else
        {
        SymbolTable& sym = SymbolTable::getInstance();
        SymbolTableEntry* e = sym.getEntry(operand.sym);
        if (!e) { sym.addEntry(operand.sym); e = sym.getEntry(operand.sym); }
        if (e->isDefined) {
            sec()->addSymbolToPool(operand.sym);
        } else {
            sym.addForwardReference(operand.sym,
                sec()->getLocationCounter(), sec()->getNdx(), false);
        }
        sec()->formInstruction(JMP, JMP_BGT_MOD | JMP_MEM_MOD, PC, regD, regS, 0);
    }
}

//Stack ops

void Instruction::pushHandler() {
    sec()->formInstruction(STORE, STORE_PUSH_MOD, SP, SP, regS, -4);
}

void Instruction::popHandler() {
    sec()->formInstruction(LOAD, LOAD_POP_MOD, regD, SP, 0, 4);
}

//arithmetic logic shift exchange

void Instruction::xchgHandler() { sec()->formInstruction(XCHG, 0, 0, regS, regD, 0); }

void Instruction::addHandler() { sec()->formInstruction(ARITM, ARITM_ADD_MOD, regD, regD, regS, 0); }
void Instruction::subHandler() { sec()->formInstruction(ARITM, ARITM_SUB_MOD, regD, regD, regS, 0); }
void Instruction::mulHandler() { sec()->formInstruction(ARITM, ARITM_MUL_MOD, regD, regD, regS, 0); }
void Instruction::divHandler() { sec()->formInstruction(ARITM, ARITM_DIV_MOD, regD, regD, regS, 0); }

void Instruction::notHandler() { sec()->formInstruction(LOGIC, LOGIC_NOT_MOD, regS, regS, 0, 0); }
void Instruction::andHandler() { sec()->formInstruction(LOGIC, LOGIC_AND_MOD, regD, regD, regS, 0); }
void Instruction::orHandler()  { sec()->formInstruction(LOGIC, LOGIC_OR_MOD,  regD, regD, regS, 0); }
void Instruction::xorHandler() { sec()->formInstruction(LOGIC, LOGIC_XOR_MOD, regD, regD, regS, 0); }

void Instruction::shlHandler() { sec()->formInstruction(SHIFT, SHIFT_SHL_MOD, regD, regD, regS, 0); }
void Instruction::shrHandler() { sec()->formInstruction(SHIFT, SHIFT_SHR_MOD, regD, regD, regS, 0); }

void Instruction::csrrdHandler() {
    // regD = gpr dest, operand.reg = csr index (status=0, handler=1, cause=2)
    sec()->formInstruction(LOAD, LOAD_CSRRD_MOD, regD, operand.reg, 0, 0);
}

void Instruction::csrwrHandler() {
    // regS = gpr source, operand.reg = csr index
    sec()->formInstruction(LOAD, LOAD_CSRWR_MOD, operand.reg, regS, 0, 0);
}

void Instruction::ldHandler() {
    SymbolTable& sym = SymbolTable::getInstance();

    switch (operand.type) {

        case OP_IMMED:
            if (operand.sym == nullptr) {
              // ld $literal, %regD   →   regD <= literal
                int literal= operand.num;
				if (Assembler::valueWithin12BitRange(literal)){
					sec()->formInstruction(LOAD,LOAD_REGDIR_MOD,regD,0,0,static_cast<int16_t>(literal));
				}
				else{
					// regD <= mem32[PC+pool_offset]
                    sec()->addLiteralToPool(literal);
					sec()->formInstruction(LOAD,LOAD_REGIND_MOD,regD,PC,0,0);
				}
              } else {
                // ld $symbol, %regD    →   regD <= adr/val symbol
                SymbolTableEntry *e = sym.getEntry(operand.sym);
                if (!e){
                    sym.addEntry(operand.sym);e=sym.getEntry(operand.sym);
                }
                if(e->isDefined){
                sec()->addSymbolToPool(operand.sym);
                }
                else{
                    sym.addForwardReference(operand.sym,sec()->getLocationCounter(),sec()->getNdx(),false);
                }
                sec()->formInstruction(LOAD,LOAD_REGIND_MOD,regD,PC,0,0);
            }
            break;

        case OP_MEM:
            if (operand.sym == nullptr) {
                // ld literal, %regD   →   regD <= mem32[literal]
                int addr = operand.num;
                if (Assembler::valueWithin12BitRange(addr)) {
                    sec()->formInstruction(LOAD, LOAD_REGIND_MOD, regD, 0, 0, static_cast<int16_t>(addr));
                } else {
					sec()->addLiteralToPool(addr);
                    sec()->formInstruction(LOAD, LOAD_REGDIR_MOD, regD, PC, 0, 0);
                    sec()->formInstruction(LOAD, LOAD_REGIND_MOD, regD, regD, 0, 0);
                }
            } else {
                // ld symbol, %regD    →   regD <= mem32[symbol]
                SymbolTableEntry* e = sym.getEntry(operand.sym);
                if (!e) { sym.addEntry(operand.sym); e = sym.getEntry(operand.sym); }
                sec()->addSymbolToPool(operand.sym);
                sec()->formInstruction(LOAD, LOAD_REGIND_MOD, regD, PC, 0, 0);
                sec()->formInstruction(LOAD, LOAD_REGIND_MOD, regD, regD, 0, 0);
            }
            break;

        case OP_REGDIR:
            // ld %reg, %regD   →   regD <= reg
            sec()->formInstruction(LOAD, LOAD_REGDIR_MOD, regD, operand.reg, 0, 0);
            break;

        case OP_REGIND:
            // ld [%reg], %regD   →   regD <= mem32[reg]
            sec()->formInstruction(LOAD, LOAD_REGIND_MOD, regD, operand.reg, 0, 0);
            break;

        case OP_REGINDPOM:
            if (operand.sym == nullptr) {
                // ld [%reg + literal], %regD   →   regD <= mem32[reg + literal]
                if (!Assembler::valueWithin12BitRange(operand.num)) {
                    std::cerr << "Error: ld displacement too large for 12-bit range." << std::endl;
                } else {
                    sec()->formInstruction(LOAD, LOAD_REGIND_MOD, regD, operand.reg, 0, static_cast<int16_t>(operand.num));
                }
            } else {
                // ld [%reg + symbol], %regD   →   regD <= mem32[reg IMPOSSIBLE TO DO WITHOUT .equ
            }
            break;
		}
}


void Instruction::stHandler() {
    Section* s = sec();
    SymbolTable& sym = SymbolTable::getInstance();

    switch (operand.type) {

        case OP_IMMED:   // st %reg, $literal  /  st %reg, $symbol
        case OP_REGDIR:  // st %reg, %rX
            std::cerr << "Error: st cannot use immediate or register as destination." << std::endl;
            break;

        case OP_MEM:
            if (operand.sym == nullptr) {
                // st %regS, literal   →   mem32[literal] <= regS
                int addr = operand.num;
                if (Assembler::valueWithin12BitRange(addr)) {
                    s->formInstruction(STORE, STORE_MEMDIR_MOD, 0, 0, regS, static_cast<int16_t>(addr));
                } else {
                    s->addLiteralToPool(addr);
                    s->formInstruction(STORE, STORE_MEMIND_MOD, PC, 0, regS, 0);
                }
            }
            else{
            // else: st %regS, symbol  →  TODO: symbol relocation
                SymbolTableEntry* e = sym.getEntry(operand.sym);
                if(!e){sym.addEntry(operand.sym);e=sym.getEntry(operand.sym);}
                if(e->isDefined){
                    sec()->addSymbolToPool(operand.sym);
                }
                else{
                    sym.addForwardReference(operand.sym,sec()->getLocationCounter(),sec()->getNdx(),false);
                }
                s->formInstruction(STORE,STORE_MEMIND_MOD,PC,0,regS,0);
             }
            break;

        case OP_REGIND:
            // st %regS, [%rX]   →   mem32[rX] <= regS
            s->formInstruction(STORE, STORE_MEMDIR_MOD, operand.reg, 0, regS, 0);
            break;

        case OP_REGINDPOM:
            if (operand.sym == nullptr) {
                // st %regS, [%rX + literal]   →   mem32[rX + literal] <= regS
                if (!Assembler::valueWithin12BitRange(operand.num)) {
                    std::cerr << "Error: st displacement too large for 12-bit range." << std::endl;
                } else {
                    s->formInstruction(STORE, STORE_MEMDIR_MOD, operand.reg, 0, regS, static_cast<int16_t>(operand.num));
                }
            }
            // else: st %regS, [%rX + symbol]  →  not implemented (no .equ)
            break;
    }
}

void LabelComponent::process(){
	addLabel(this->label);
}