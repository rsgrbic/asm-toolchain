all: assembler

parser.tab.cpp parser.tab.hpp: ./misc/parser.ypp
	bison -d -o parser.tab.cpp ./misc/parser.ypp

lexer.cpp: ./misc/lexer.l parser.tab.hpp
	flex -o lexer.cpp ./misc/lexer.l

assembler: parser.tab.cpp lexer.cpp src/main.cpp
	g++ -Iinc -I. parser.tab.cpp lexer.cpp ./src/* -o assembler

clean:
	rm -f assembler lexer.cpp parser.tab.cpp parser.tab.hpp
