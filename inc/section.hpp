#ifndef _SECTION_HPP
#define _SECTION_HPP

#include <string>
#include <vector>
#include <cstdint>
#include <unordered_map>

class Section {
    std::string name;
    int ndx;
    int locationCounter;
    std::unordered_map<int, std::vector<int>> literalPool;
    std::vector<uint8_t> content;
public:
    Section(const std::string& name, int ndx);

    void addByte(int8_t byte);
    void addQuadbyte(int32_t quad);
    std::string getName() const { return name; }
    int getNdx() const { return ndx; }
    int getLocationCounter() const { return locationCounter; }
    std::vector<uint8_t>& getContent() { return content; }

    void addLiteralToPool(int literal);
    void changeDisplacementInInstruction(int32_t disp, uint32_t newDisp);
    std::unordered_map<int, std::vector<int>>& getLiteralPool();

void addInstruction(uint32_t ins);
};

#endif