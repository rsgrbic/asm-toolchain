#ifndef _SECTION_HPP
#define _SECTION_HPP

#include <string>
#include <vector>
#include <cstdint>
#include <fstream>
#include <unordered_map>
#include <relocation_table.hpp>


// --- 4-bit hardware opcodes (bits 31:28 of instruction word) ---
enum Opcode {
    HALT  = 0x0,
    INT   = 0x1,
    CALL  = 0x2,
    JMP   = 0x3,   // also BEQ/BNE/BGT via mod
    XCHG  = 0x4,
    ARITM = 0x5,   // ADD/SUB/MUL/DIV via mod
    LOGIC = 0x6,   // NOT/AND/OR/XOR via mod
    SHIFT = 0x7,   // SHL/SHR via mod
    STORE = 0x8,
    LOAD  = 0x9
};

// --- Mod field values ---
enum LOADMod {
    LOAD_CSRRD_MOD  = 0x0,  // gpr[A] <= csr[B]
    LOAD_REGDIR_MOD = 0x1,  // gpr[A] <= gpr[B] + D          (also literal pool)
    LOAD_REGIND_MOD = 0x2,  // gpr[A] <= mem32[gpr[B]+gpr[C]+D]
    LOAD_POP_MOD    = 0x3,  // gpr[A] <= mem32[gpr[B]]; gpr[B]+=D
    LOAD_CSRWR_MOD  = 0x4,  // csr[A] <= gpr[B]
    LOAD_CSR_MOD    = 0x5,  // csr[A] <= csr[B] | D
    LOAD_CSRIND_MOD = 0x6,  // csr[A] <= mem32[gpr[B]+gpr[C]+D]
    LOAD_CSRPOP_MOD = 0x7   // csr[A] <= mem32[gpr[B]]; gpr[B]+=D
};

enum STOREMod {
    STORE_MEMDIR_MOD = 0x0,  // mem32[gpr[A]+gpr[B]+D] <= gpr[C]
    STORE_PUSH_MOD   = 0x1,  // gpr[A]+=D; mem32[gpr[A]] <= gpr[C]
    STORE_MEMIND_MOD = 0x2   // mem32[mem32[gpr[A]+gpr[B]+D]] <= gpr[C]
};

enum JMPMod {
    JMP_REG_MOD  = 0x0,  // pc <= gpr[A] + D
    JMP_BEQ_MOD  = 0x1,  // if (gpr[B]==gpr[C]) pc <= gpr[A]+D
    JMP_BNE_MOD  = 0x2,  // if (gpr[B]!=gpr[C]) pc <= gpr[A]+D
    JMP_BGT_MOD  = 0x3,  // if (gpr[B] signed> gpr[C]) pc <= gpr[A]+D
    JMP_MEM_MOD  = 0x8,  // pc <= mem32[gpr[A]+D]           (OR'd with BEQ/BNE/BGT)
};

enum CALLMod {
    CALL_REG_MOD = 0x0,  // push pc; pc <= gpr[A]+gpr[B]+D
    CALL_MEM_MOD = 0x1   // push pc; pc <= mem32[gpr[A]+gpr[B]+D]
};

enum ARITMMod {
    ARITM_ADD_MOD = 0x0,
    ARITM_SUB_MOD = 0x1,
    ARITM_MUL_MOD = 0x2,
    ARITM_DIV_MOD = 0x3
};

enum LOGICMod {
    LOGIC_NOT_MOD = 0x0,  // gpr[A] <= ~gpr[B]
    LOGIC_AND_MOD = 0x1,  // gpr[A] <= gpr[B] & gpr[C]
    LOGIC_OR_MOD  = 0x2,  // gpr[A] <= gpr[B] | gpr[C]
    LOGIC_XOR_MOD = 0x3   // gpr[A] <= gpr[B] ^ gpr[C]
};

enum SHIFTMod {
    SHIFT_SHL_MOD = 0x0,  // gpr[A] <= gpr[B] << gpr[C]
    SHIFT_SHR_MOD = 0x1   // gpr[A] <= gpr[B] >> gpr[C]
};


class Section {
    std::string name;
    int ndx;
    int locationCounter;
    RelocationTable relocTable;
    std::unordered_map<int, std::vector<int>> literalPool;
    std::unordered_map<std::string, std::vector<int>> symbolPool;

    std::vector<uint8_t> content;
public:
    Section(const std::string& name, int ndx);

    void printToOutput(std::ofstream& out);
    RelocationTable& getRelocationTable(){return relocTable;}

    void addByte(int8_t byte);
    void addQuadbyte(int32_t quad);

    void addQuadbyteAtOffset(int32_t value, uint32_t offset);
    std::string getName() const { return name; }
    int getNdx() const { return ndx; }
    int getLocationCounter() const { return locationCounter; }
    std::vector<uint8_t>& getContent() { return content; }
    //pool
    void addLiteralToPool(int literal);
    void changeDisplacementInInstruction(int32_t disp, uint32_t newDisp);
    std::unordered_map<int, std::vector<int>>& getLiteralPool();

    //symbols
    void addSymbolToPool(const std::string& name) {
    symbolPool[name].push_back(locationCounter);
}
std::unordered_map<std::string, std::vector<int>>& getsymbolPool() { return symbolPool; }

    void addInstruction(uint32_t ins);
    void formInstruction(Opcode opcode, uint8_t mod, uint8_t regA, uint8_t regB, uint8_t regC, int16_t disp);
    void formInstructionWithLiteral(Opcode opcode, uint8_t mod, uint8_t regA,
                                     uint8_t regB, uint8_t regC, int32_t literal);
};

#endif