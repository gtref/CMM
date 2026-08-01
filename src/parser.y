/* Make AST types visible to both parser.c and parser.tab.h */
%code requires {
    #include "ast.h"
}

%{
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "ast.h"   /* needed in parser.tab.c for ASTNode, TokenKind */

extern int yylex(void);
extern int current_line;
void yyerror(const char *s);

/* Global AST root */
ASTNode *root = NULL;
%}

%union {
    int int_val;
    double float_val;
    char *str_val;
    TokenKind token_kind;
    struct ASTNode *node;
}

%token <str_val> TOK_IDENTIFIER TOK_STRING_LITERAL TOK_CHAR_LITERAL
%token <int_val> TOK_NUMBER
%token <float_val> TOK_FLOAT_LITERAL

%token TOK_IF TOK_ELSE TOK_SWITCH TOK_GOTO TOK_RETURN TOK_CALL TOK_JUMP TOK_FOREIGN TOK_IMPORT TOK_EXPORT
%token TOK_DATA TOK_STACK TOK_NOALIAS TOK_SECTION TOK_PRAGMA
%token TOK_WORD1 TOK_WORD2 TOK_WORD4 TOK_WORD8 TOK_FLOAT4 TOK_FLOAT8
%token TOK_BITS8 TOK_BITS16 TOK_BITS32 TOK_BITS64
%token TOK_ALIGN1 TOK_ALIGN2 TOK_ALIGN4 TOK_ALIGN8
%token TOK_LPAREN TOK_RPAREN TOK_LBRACE TOK_RBRACE TOK_LBRACKET TOK_RBRACKET
%token TOK_COMMA TOK_SEMICOLON TOK_COLON TOK_COLONCOLON
%token TOK_ASSIGN TOK_PLUS TOK_MINUS TOK_STAR TOK_SLASH TOK_PERCENT
%token TOK_EQEQ TOK_NEQ TOK_LT TOK_LTE TOK_GT TOK_GTE
%token TOK_AMP TOK_PIPE TOK_CARET TOK_TILDE TOK_ERROR TOK_EOF

%type <node> program decl_list decl procedure func_body stmt_list stmt var_decl expr primary
%type <token_kind> type_specifier

%left TOK_EQEQ TOK_NEQ TOK_LT TOK_LTE TOK_GT TOK_GTE
%left TOK_PLUS TOK_MINUS
%left TOK_STAR TOK_SLASH

%%

program:
    decl_list                               { root = $1; }
    ;

decl_list:
    decl_list decl                          { $$ = create_list_node($1, $2); }
  | decl                                    { $$ = $1; }
  ;

decl:
    procedure                               { $$ = $1; }
  | var_decl                                { $$ = $1; }
  | TOK_EXPORT TOK_IDENTIFIER TOK_SEMICOLON { $$ = create_export_node($2); free($2); }
  | TOK_IMPORT TOK_IDENTIFIER TOK_SEMICOLON { $$ = create_import_node($2); free($2); }
  ;

procedure:
    TOK_IDENTIFIER TOK_LPAREN TOK_RPAREN TOK_LBRACE func_body TOK_RBRACE {
        $$ = create_func_node($1, $5);
        free($1);
    }
  ;

func_body:
    stmt_list                               { $$ = $1; }
  ;

stmt_list:
    stmt_list stmt                          { $$ = create_list_node($1, $2); }
  | stmt                                    { $$ = $1; }
  ;

stmt:
    var_decl                                { $$ = $1; }
  | TOK_IDENTIFIER TOK_ASSIGN expr TOK_SEMICOLON {
        $$ = create_assign_node($1, $3);
        free($1);
    }
  | TOK_IF TOK_LPAREN expr TOK_RPAREN TOK_LBRACE stmt_list TOK_RBRACE {
        $$ = create_if_node($3, $6, NULL);
    }
  | TOK_IF TOK_LPAREN expr TOK_RPAREN TOK_LBRACE stmt_list TOK_RBRACE TOK_ELSE TOK_LBRACE stmt_list TOK_RBRACE {
        $$ = create_if_node($3, $6, $10);
    }
  | TOK_GOTO TOK_IDENTIFIER TOK_SEMICOLON {
        $$ = create_goto_node($2);
        free($2);
    }
  | TOK_IDENTIFIER TOK_COLON {
        $$ = create_label_node($1);
        free($1);
    }
  | TOK_RETURN expr TOK_SEMICOLON {
        $$ = create_return_node($2);
    }
  | TOK_RETURN TOK_SEMICOLON {
        $$ = create_return_node(NULL);
    }
  | TOK_FOREIGN TOK_STRING_LITERAL TOK_IDENTIFIER TOK_LPAREN expr TOK_RPAREN TOK_SEMICOLON {
        $$ = create_foreign_call_node($2, $3, $5);
        free($2); free($3);
    }
  ;

var_decl:
    type_specifier TOK_IDENTIFIER TOK_ASSIGN expr TOK_SEMICOLON {
        $$ = create_var_decl_node($1, $2, $4);
        free($2);
    }
  | type_specifier TOK_IDENTIFIER TOK_SEMICOLON {
        $$ = create_var_decl_node($1, $2, NULL);
        free($2);
    }
  ;

type_specifier:
    TOK_BITS8   { $$ = TOK_BITS8; }
  | TOK_BITS16  { $$ = TOK_BITS16; }
  | TOK_BITS32  { $$ = TOK_BITS32; }
  | TOK_BITS64  { $$ = TOK_BITS64; }
  ;

expr:
    expr TOK_PLUS expr                      { $$ = create_binary_node(TOK_PLUS, $1, $3); }
  | expr TOK_MINUS expr                     { $$ = create_binary_node(TOK_MINUS, $1, $3); }
  | expr TOK_STAR expr                      { $$ = create_binary_node(TOK_STAR, $1, $3); }
  | expr TOK_SLASH expr                     { $$ = create_binary_node(TOK_SLASH, $1, $3); }
  | expr TOK_EQEQ expr                      { $$ = create_binary_node(TOK_EQEQ, $1, $3); }
  | expr TOK_NEQ expr                       { $$ = create_binary_node(TOK_NEQ, $1, $3); }
  | expr TOK_LT expr                        { $$ = create_binary_node(TOK_LT, $1, $3); }
  | expr TOK_GT expr                        { $$ = create_binary_node(TOK_GT, $1, $3); }
  | primary                                 { $$ = $1; }
  ;

primary:
    TOK_NUMBER                              { $$ = create_int_node($1); }
  | TOK_IDENTIFIER                          { $$ = create_var_ref_node($1); free($1); }
  | TOK_LPAREN expr TOK_RPAREN              { $$ = $2; }
  ;

%%

void yyerror(const char *s) {
    fprintf(stderr, "Syntax Error on line %d: %s\n", current_line, s);
    exit(1);
}
