#include <iostream>
#include <cstdio>
#include <vector>

#include "parser.hpp"
#include "parser.tab.hpp"
#include "symbol_table.hpp"
#include "assembler.hpp"
#include <unistd.h>

extern int yyparse();
extern FILE* yyin;
extern unsigned lineNum;
extern void yylex_destroy();

int main(int argc, char** argv) {
    
    std::string outputFile;
    int opt;
    while ((opt = getopt(argc, argv, "o:")) != -1) {
    if (opt == 'o') {
        outputFile = optarg;
    } else {
        std::cerr << "Usage: " << argv[0] << " [-o output.o] input.s\n";
        return 1;
        }
    }
    if (optind >= argc) {
    std::cerr << "Usage: " << argv[0] << " [-o output.o] input.s\n";
    return 1;
    }
    std::string inputFile = argv[optind];
    
    yyin = fopen(inputFile.c_str(), "r");
    if (!yyin) { perror("fopen"); return 1; }

    // Init state
    lineNum = 1;
    Assembler::getInstance().initialize();

    // Parse
    std::vector<CodeComponent*> components;
    int result = yyparse(components);

    if (result != 0) {
        std::cerr << "Parsing failed." << std::endl;
        return result;
    }

    // Process — one pass
    for (auto* c : components){
        c->process();
        if(Assembler::getInstance().isFinished()) break;
    }

    Assembler::getInstance().promoteGlobalsToExtern();
    if (!Assembler::getInstance().checkIfAllSymbolsAreDefined()){
        std::cerr<< "Not all symbols are defined"<<std::endl;
        return 1;
    }
    Assembler::getInstance().insertLiteralPools();
    Assembler::getInstance().insertSymbolPools();
    std::cout << "Assembling successful (" << components.size() << " components)." << std::endl;
    
    Assembler::getInstance().writeObjectFile(outputFile);
    std::cout << "Wrote " << outputFile << "\n";

    // Cleanup
    for (auto* c : components) delete c;
    fclose(yyin);
    yylex_destroy();
    return 0;
}



