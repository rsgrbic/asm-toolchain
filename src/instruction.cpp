#include "./inc/parser.hpp"
#include "./inc/assembler.hpp"
#include "./inc/symbol_table.hpp"
// #include <iostream>


CodeComponent::~CodeComponent() =default;

void Instruction::process() {
    if (type == INS_LD && operand.type == OP_IMMED && operand.sym == nullptr) {
        // ld $0xFFFFFEFE, %sp  →  gprD <= literal
        int literal = operand.num;
        Assembler& asmbl = Assembler::getInstance();
        Section* sec = asmbl.getCurrentSection();
        //small literal
        if (Assembler::valueWithin12BitRange(literal)) {
            uint32_t ins = (0x9 << 28) | (0x1 << 24) | (regD << 20) | (0 << 16) | (0 << 12) | (literal & 0xFFF);
            sec->addInstruction(ins);
        } else {
          //large literal
            // LOAD MOD=2:  gpr[A] <= mem32[gpr[B] + gpr[C] + D]
            // RegDest <= mem32[PC+ D(patched)]
            sec->addLiteralToPool(literal);
            uint32_t ins = (0x9 << 28) | (0x2 << 24) | (regD << 20) | (15 << 16) | (0 << 12) | 0;
            sec->addInstruction(ins);
          }
    }
}

// void Directive::process(){

//   switch(this->type){
//     case DIR_GLOBAL: std::cout<< "Type: DIR GLOBAL "<< " SYMBOLS : " ;
//           for (auto &elem : *syms){
//             if (elem.type == SYM_SYM)
//             std::cout << " " << elem.data.sym;
//         else
//             std::cout << " " << elem.data.num;
//           }
//           std::cout<<std::endl;
//       break;
//     case DIR_SECTION:
//           std::cout<< "Type: DIR SECTION " << "Name: " <<this->sym<<std::endl;
//           break;
//   }
// }

void LabelComponent::process() {
  addLabel(label);
}

