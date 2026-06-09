#include "section.hpp"

Section::Section(const std::string& name, int ndx)
    : name(name), ndx(ndx), locationCounter(0) {}

void Section::addByte(int8_t byte) {
    content.push_back(byte);
    locationCounter++;
}

void Section::addQuadbyte(int32_t quad) {
    for (int i = 0; i < 4; i++)
        content.push_back((quad >> (i * 8)) & 0xFF);
    locationCounter += 4;
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