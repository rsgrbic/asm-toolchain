/* A Bison parser, made by GNU Bison 3.5.1.  */

/* Bison interface for Yacc-like parsers in C

   Copyright (C) 1984, 1989-1990, 2000-2015, 2018-2020 Free Software Foundation,
   Inc.

   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <http://www.gnu.org/licenses/>.  */

/* As a special exception, you may create a larger work that contains
   part or all of the Bison parser skeleton and distribute that work
   under terms of your choice, so long as that work isn't itself a
   parser generator using the skeleton or a modified version thereof
   as a parser skeleton.  Alternatively, if you modify or redistribute
   the parser skeleton itself, you may (at your option) remove this
   special exception, which will cause the skeleton and the resulting
   Bison output files to be licensed under the GNU General Public
   License without this special exception.

   This special exception was added by the Free Software Foundation in
   version 2.2 of Bison.  */

/* Undocumented macros, especially those whose name start with YY_,
   are private implementation details.  Do not rely on them.  */

#ifndef YY_YY_PARSER_TAB_HPP_INCLUDED
# define YY_YY_PARSER_TAB_HPP_INCLUDED
/* Debug traces.  */
#ifndef YYDEBUG
# define YYDEBUG 0
#endif
#if YYDEBUG
extern int yydebug;
#endif
/* "%code requires" blocks.  */
#line 1 "./misc/parser.ypp"

    #include <vector>
    #include "./inc/parser.hpp"

#line 53 "parser.tab.hpp"

/* Token type.  */
#ifndef YYTOKENTYPE
# define YYTOKENTYPE
  enum yytokentype
  {
    PLUS = 258,
    DOLLAR = 259,
    LBRACKET = 260,
    RBRACKET = 261,
    COMMA = 262,
    COMMENT = 263,
    ENDL = 264,
    DIRECTIVE_GLOBAL = 265,
    DIRECTIVE_EXTERN = 266,
    DIRECTIVE_SECTION = 267,
    DIRECTIVE_WORD = 268,
    DIRECTIVE_SKIP = 269,
    DIRECTIVE_ASCII = 270,
    DIRECTIVE_END = 271,
    INSTRUCTION_HALT = 272,
    INSTRUCTION_INT = 273,
    INSTRUCTION_IRET = 274,
    INSTRUCTION_CALL = 275,
    INSTRUCTION_RET = 276,
    INSTRUCTION_JMP = 277,
    INSTRUCTION_BEQ = 278,
    INSTRUCTION_BNE = 279,
    INSTRUCTION_BGT = 280,
    INSTRUCTION_PUSH = 281,
    INSTRUCTION_POP = 282,
    INSTRUCTION_XCHG = 283,
    INSTRUCTION_ADD = 284,
    INSTRUCTION_SUB = 285,
    INSTRUCTION_MUL = 286,
    INSTRUCTION_DIV = 287,
    INSTRUCTION_NOT = 288,
    INSTRUCTION_AND = 289,
    INSTRUCTION_OR = 290,
    INSTRUCTION_XOR = 291,
    INSTRUCTION_SHL = 292,
    INSTRUCTION_SHR = 293,
    INSTRUCTION_LD = 294,
    INSTRUCTION_ST = 295,
    INSTRUCTION_CSRRD = 296,
    INSTRUCTION_CSRWR = 297,
    REG = 298,
    CSR = 299,
    NUM = 300,
    SYM = 301,
    STRING = 302,
    LABEL = 303
  };
#endif

/* Value type.  */
#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED
union YYSTYPE
{
#line 19 "./misc/parser.ypp"

    int reg;
    int num;
    char* sym;
    char* label;
    char* str;
    std::vector<SymbolListElement>* symVec;
    Operand operand;
    Instruction* instruction;
    Directive* directive;

#line 125 "parser.tab.hpp"

};
typedef union YYSTYPE YYSTYPE;
# define YYSTYPE_IS_TRIVIAL 1
# define YYSTYPE_IS_DECLARED 1
#endif


extern YYSTYPE yylval;

int yyparse (std::vector<CodeComponent*>& components);

#endif /* !YY_YY_PARSER_TAB_HPP_INCLUDED  */
