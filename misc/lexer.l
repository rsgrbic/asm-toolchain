%option noyywrap

%{

#include "parser.tab.hpp"
#include <vector>
#include "inc/parser.hpp"
unsigned lineNum=1;

%}

%%
[ \t]                               ;
"+"                                 { return PLUS; }
"$"                                 { return DOLLAR; }
"["                                 { return LBRACKET; }
"]"                                 { return RBRACKET; }
","                                 { return COMMA; }
#.*                                 { return COMMENT; }
halt                                { return INSTRUCTION_HALT; }
int                                 { return INSTRUCTION_INT; }
iret                                { return INSTRUCTION_IRET; }
call                                { return INSTRUCTION_CALL; }
ret                                 { return INSTRUCTION_RET; }
jmp                                 { return INSTRUCTION_JMP; }
beq                                 { return INSTRUCTION_BEQ; }
bne                                 { return INSTRUCTION_BNE; }
bgt                                 { return INSTRUCTION_BGT; }
push                                { return INSTRUCTION_PUSH; }
pop                                 { return INSTRUCTION_POP; }
xchg                                { return INSTRUCTION_XCHG; }
add                                 { return INSTRUCTION_ADD; }
sub                                 { return INSTRUCTION_SUB; }
mul                                 { return INSTRUCTION_MUL; }
div                                 { return INSTRUCTION_DIV; }
not                                 { return INSTRUCTION_NOT; }
and                                 { return INSTRUCTION_AND; }
or                                  { return INSTRUCTION_OR; }
xor                                 { return INSTRUCTION_XOR; }
shl                                 { return INSTRUCTION_SHL; }
shr                                 { return INSTRUCTION_SHR; }
ld                                  { return INSTRUCTION_LD; }
st                                  { return INSTRUCTION_ST; }
csrrd                               { return INSTRUCTION_CSRRD; }
csrwr                               { return INSTRUCTION_CSRWR; }
".global"                           { return DIRECTIVE_GLOBAL; }
".extern"                           { return DIRECTIVE_EXTERN; }
".section"                          { return DIRECTIVE_SECTION; }
".word"                             { return DIRECTIVE_WORD; }
".skip"                             { return DIRECTIVE_SKIP; }
".ascii"                            { return DIRECTIVE_ASCII; }
".end"                              { return DIRECTIVE_END; }
[a-zA-Z_][a-zA-Z0-9_]*[ \t]*:       {
    yylval.label = strdup(yytext);
    if (yylval.label == NULL) {
        return LABEL;
    }
    int i = 0;
    while (yylval.label[i] != '\0') {
        char c = yylval.label[i];
        if (c == '\t' || c == ' ' || c == ':') {
            yylval.label[i] = '\0';
            break;
        }
        ++i;
    }
    return LABEL;
}

%r([0-9]|1[0-5])                     { yylval.reg = atoi(yytext + 2); return REG; }
%sp                                  { yylval.reg = 14; return REG; }
%pc                                  { yylval.reg = 15; return REG; }
%status                              { yylval.reg = 20; return CSR; }
%handler                             { yylval.reg = 21; return CSR; }
%cause                               { yylval.reg = 22; return CSR; }
0                                   { yylval.num = 0; return NUM; }
[1-9][0-9]*                         { yylval.num = atoi(yytext); return NUM; }
0[0-7]+                             { yylval.num = strtol(yytext + 1, NULL, 8); return NUM; }
0x[0-9a-fA-F]+                      { yylval.num = strtol(yytext + 2, NULL, 16); return NUM; }
\"[^"]+\"                           { yytext[strlen(yytext)-1] = '\0'; yylval.sym = strdup(yytext + 1); return STRING; }
[a-zA-Z_.][a-zA-Z0-9_.]*            { yylval.sym = strdup(yytext); return SYM; }
\n                                  { ++lineNum; BEGIN(INITIAL); return ENDL; }
.                                   ;
%%