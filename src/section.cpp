#include "section.hpp"
#include "assembler.hpp"
#include <iomanip>
Section::Section(const std::string& name, int ndx)
    : name(name), ndx(ndx), locationCounter(0) {}

void Section::printToOutput(std::ofstream &out)
{
    out << "SECTION " << name << " " << ndx << " " << locationCounter << "\n";
    int count = 0;
    for (uint8_t byte : content) {
        out << std::hex << std::uppercase << std::setw(2) << std::setfill('0')
            << static_cast<int>(byte) << " ";
        count++;
        if (count == 8) {
            out << "\n";
            count = 0;
        }
    }
    if (count != 0) {
        out << "\n";
    }
    out << std::dec << "END_SECTION\n";
    relocTable.printToOutput(out);
}

void Section::addByte(int8_t byte) {
    content.push_back(byte);
    locationCounter++;
}

void Section::addQuadbyte(int32_t quad) {
    for (int i = 0; i < 4; i++)
        content.push_back((quad >> (i * 8)) & 0xFF);
    locationCounter += 4;
}

void Section::addQuadbyteAtOffset(int32_t value, uint32_t offset) {
    content[offset]     = value & 0xFF;
    content[offset + 1] = (value >> 8) & 0xFF;
    content[offset + 2] = (value >> 16) & 0xFF;
    content[offset + 3] = (value >> 24) & 0xFF;
}

void Section::addLiteralToPool(int literal) {
    if (this->literalPool.find(literal) == literalPool.end())
        literalPool[literal] = std::vector<int>();
    literalPool[literal].push_back(locationCounter);
}

void Section::changeDisplacementInInstruction(int32_t offset, uint32_t newDisp) {
    // Byte +0 = Disp[7:0]  (clear + set)
    content[offset] = newDisp & 0xFF;
    // Byte +1 = RegC[3:0] | Disp[11:8]  (preserve upper nibble)
    content[offset + 1] = (content[offset + 1] & 0xF0) | ((newDisp >> 8) & 0x0F);
}

std::unordered_map<int, std::vector<int>>& Section::getLiteralPool() {
    return literalPool;
}

void Section::addInstruction(uint32_t ins) {
    // Little-endian: LSB first
    content.push_back(ins & 0xFF);         // Disp[7:0]
    content.push_back((ins >> 8) & 0xFF);  // RegC[3:0] | Disp[11:8]
    content.push_back((ins >> 16) & 0xFF); // RegA[3:0] | Mod[3:0]
    content.push_back((ins >> 24) & 0xFF); // OC[3:0] | ... 
    locationCounter += 4;
}
void Section::formInstruction(Opcode opcode, uint8_t mod, uint8_t regA, uint8_t regB, uint8_t regC, int16_t disp){
     uint32_t ins = (static_cast<uint32_t>(opcode) << 28) |
                   (static_cast<uint32_t>(mod)    << 24) |
                   (static_cast<uint32_t>(regA)   << 20) |
                   (static_cast<uint32_t>(regB)   << 16) |
                   (static_cast<uint32_t>(regC)   << 12) |
                   (disp & 0xFFF);
    addInstruction(ins);
}

void Section::formInstructionWithLiteral(Opcode opcode, uint8_t mod,
                                          uint8_t regA, uint8_t regB,
                                          uint8_t regC, int32_t literal) {
    if (Assembler::valueWithin12BitRange(literal)) {
        formInstruction(opcode, mod, regA, regB, regC, static_cast<uint16_t>(literal & 0xFFF));
    } else {
        switch (opcode) {
            case CALL: mod |= CALL_MEM_MOD; break;
            case JMP:  mod |= JMP_MEM_MOD;  break;
            case LOAD: case STORE: mod = LOAD_REGIND_MOD; break;
            default: break;
        }
        addLiteralToPool(literal);
        formInstruction(opcode, mod, regA, regB, regC, 0);
    }
}