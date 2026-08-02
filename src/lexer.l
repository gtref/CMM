%{
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "ast.h"
#include "parser.tab.h"

int current_line = 1;
%}

%option noyywrap

%%

[ \t\r]+                { /* Skip whitespace */ }
\n                      { current_line++; }

"//".* { /* Skip C++ style comments */ }
"--".* { /* Skip C-- style comments */ }

"if"                    { return TOK_IF; }
"else"                  { return TOK_ELSE; }
"switch"                { return TOK_SWITCH; }
"goto"                  { return TOK_GOTO; }
"return"                { return TOK_RETURN; }
"call"                  { return TOK_CALL; }
"jump"                  { return TOK_JUMP; }
"foreign"               { return TOK_FOREIGN; }
"import"                { return TOK_IMPORT; }
"export"                { return TOK_EXPORT; }

"data"                  { return TOK_DATA; }
"stack"                 { return TOK_STACK; }
"noalias"               { return TOK_NOALIAS; }
"section"               { return TOK_SECTION; }
"pragma"                { return TOK_PRAGMA; }

"align1"                { return TOK_ALIGN1; }
"align2"                { return TOK_ALIGN2; }
"align4"                { return TOK_ALIGN4; }
"align8"                { return TOK_ALIGN8; }

"word1"                 { return TOK_WORD1; }
"word2"                 { return TOK_WORD2; }
"word4"                 { return TOK_WORD4; }
"word8"                 { return TOK_WORD8; }
"float4"                { return TOK_FLOAT4; }
"float8"                { return TOK_FLOAT8; }

"bits8"                 { return TOK_BITS8; }
"bits16"                { return TOK_BITS16; }
"bits32"                { return TOK_BITS32; }
"bits64"                { return TOK_BITS64; }

0[xX][0-9a-fA-F]+       { yylval.int_val = (int)strtol(yytext, NULL, 16); return TOK_NUMBER; }
[0-9]+\.[0-9]+          { yylval.float_val = atof(yytext); return TOK_FLOAT_LITERAL; }
[0-9]+                  { yylval.int_val = atoi(yytext); return TOK_NUMBER; }

\"([^"\\]|\\.)*\"       { yylval.str_val = strdup(yytext); return TOK_STRING_LITERAL; }
'([^'\\]|\\.)'          { yylval.str_val = strdup(yytext); return TOK_CHAR_LITERAL; }

[a-zA-Z_$][a-zA-Z0-9_$]* { yylval.str_val = strdup(yytext); return TOK_IDENTIFIER; }

"::"                    { return TOK_COLONCOLON; }
"=="                    { return TOK_EQEQ; }
"!="                    { return TOK_NEQ; }
"<="                    { return TOK_LTE; }
">="                    { return TOK_GTE; }

"("                     { return TOK_LPAREN; }
")"                     { return TOK_RPAREN; }
"{"                     { return TOK_LBRACE; }
"}"                     { return TOK_RBRACE; }
"["                     { return TOK_LBRACKET; }
"]"                     { return TOK_RBRACKET; }
","                     { return TOK_COMMA; }
";"                     { return TOK_SEMICOLON; }
":"                     { return TOK_COLON; }
"="                     { return TOK_ASSIGN; }
"+"                     { return TOK_PLUS; }
"-"                     { return TOK_MINUS; }
"*"                     { return TOK_STAR; }
"/"                     { return TOK_SLASH; }
"%"                     { return TOK_PERCENT; }
"<"                     { return TOK_LT; }
">"                     { return TOK_GT; }
"&"                     { return TOK_AMP; }
"|"                     { return TOK_PIPE; }
"^"                     { return TOK_CARET; }
"~"                     { return TOK_TILDE; }

.                       { fprintf(stderr, "Lexical Error on line %d: invalid token '%s'\n", current_line, yytext); return TOK_ERROR; }

%%